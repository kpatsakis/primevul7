static int sas_get_port_device(struct asd_sas_port *port)
{
	struct asd_sas_phy *phy;
	struct sas_rphy *rphy;
	struct domain_device *dev;
	int rc = -ENODEV;

	dev = sas_alloc_device();
	if (!dev)
		return -ENOMEM;

	spin_lock_irq(&port->phy_list_lock);
	if (list_empty(&port->phy_list)) {
		spin_unlock_irq(&port->phy_list_lock);
		sas_put_device(dev);
		return -ENODEV;
	}
	phy = container_of(port->phy_list.next, struct asd_sas_phy, port_phy_el);
	spin_lock(&phy->frame_rcvd_lock);
	memcpy(dev->frame_rcvd, phy->frame_rcvd, min(sizeof(dev->frame_rcvd),
					     (size_t)phy->frame_rcvd_size));
	spin_unlock(&phy->frame_rcvd_lock);
	spin_unlock_irq(&port->phy_list_lock);

	if (dev->frame_rcvd[0] == 0x34 && port->oob_mode == SATA_OOB_MODE) {
		struct dev_to_host_fis *fis =
			(struct dev_to_host_fis *) dev->frame_rcvd;
		if (fis->interrupt_reason == 1 && fis->lbal == 1 &&
		    fis->byte_count_low==0x69 && fis->byte_count_high == 0x96
		    && (fis->device & ~0x10) == 0)
			dev->dev_type = SAS_SATA_PM;
		else
			dev->dev_type = SAS_SATA_DEV;
		dev->tproto = SAS_PROTOCOL_SATA;
	} else if (port->oob_mode == SAS_OOB_MODE) {
		struct sas_identify_frame *id =
			(struct sas_identify_frame *) dev->frame_rcvd;
		dev->dev_type = id->dev_type;
		dev->iproto = id->initiator_bits;
		dev->tproto = id->target_bits;
	} else {
		/* If the oob mode is OOB_NOT_CONNECTED, the port is
		 * disconnected due to race with PHY down. We cannot
		 * continue to discover this port
		 */
		sas_put_device(dev);
		pr_warn("Port %016llx is disconnected when discovering\n",
			SAS_ADDR(port->attached_sas_addr));
		return -ENODEV;
	}

	sas_init_dev(dev);

	dev->port = port;
	switch (dev->dev_type) {
	case SAS_SATA_DEV:
		rc = sas_ata_init(dev);
		if (rc) {
			rphy = NULL;
			break;
		}
		/* fall through */
	case SAS_END_DEVICE:
		rphy = sas_end_device_alloc(port->port);
		break;
	case SAS_EDGE_EXPANDER_DEVICE:
		rphy = sas_expander_alloc(port->port,
					  SAS_EDGE_EXPANDER_DEVICE);
		break;
	case SAS_FANOUT_EXPANDER_DEVICE:
		rphy = sas_expander_alloc(port->port,
					  SAS_FANOUT_EXPANDER_DEVICE);
		break;
	default:
		pr_warn("ERROR: Unidentified device type %d\n", dev->dev_type);
		rphy = NULL;
		break;
	}

	if (!rphy) {
		sas_put_device(dev);
		return rc;
	}

	rphy->identify.phy_identifier = phy->phy->identify.phy_identifier;
	memcpy(dev->sas_addr, port->attached_sas_addr, SAS_ADDR_SIZE);
	sas_fill_in_rphy(dev, rphy);
	sas_hash_addr(dev->hashed_sas_addr, dev->sas_addr);
	port->port_dev = dev;
	dev->linkrate = port->linkrate;
	dev->min_linkrate = port->linkrate;
	dev->max_linkrate = port->linkrate;
	dev->pathways = port->num_phys;
	memset(port->disc.fanout_sas_addr, 0, SAS_ADDR_SIZE);
	memset(port->disc.eeds_a, 0, SAS_ADDR_SIZE);
	memset(port->disc.eeds_b, 0, SAS_ADDR_SIZE);
	port->disc.max_level = 0;
	sas_device_set_phy(dev, port->port);

	dev->rphy = rphy;
	get_device(&dev->rphy->dev);

	if (dev_is_sata(dev) || dev->dev_type == SAS_END_DEVICE)
		list_add_tail(&dev->disco_list_node, &port->disco_list);
	else {
		spin_lock_irq(&port->dev_list_lock);
		list_add_tail(&dev->dev_list_node, &port->dev_list);
		spin_unlock_irq(&port->dev_list_lock);
	}

	spin_lock_irq(&port->phy_list_lock);
	list_for_each_entry(phy, &port->phy_list, port_phy_el)
		sas_phy_set_target(phy, dev);
	spin_unlock_irq(&port->phy_list_lock);

	return 0;
}