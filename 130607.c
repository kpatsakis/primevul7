static void sas_probe_devices(struct asd_sas_port *port)
{
	struct domain_device *dev, *n;

	/* devices must be domain members before link recovery and probe */
	list_for_each_entry(dev, &port->disco_list, disco_list_node) {
		spin_lock_irq(&port->dev_list_lock);
		list_add_tail(&dev->dev_list_node, &port->dev_list);
		spin_unlock_irq(&port->dev_list_lock);
	}

	sas_probe_sata(port);

	list_for_each_entry_safe(dev, n, &port->disco_list, disco_list_node) {
		int err;

		err = sas_rphy_add(dev->rphy);
		if (err)
			sas_fail_probe(dev, __func__, err);
		else
			list_del_init(&dev->disco_list_node);
	}
}