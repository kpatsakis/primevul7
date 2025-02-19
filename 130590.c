static void sas_unregister_common_dev(struct asd_sas_port *port, struct domain_device *dev)
{
	struct sas_ha_struct *ha = port->ha;

	sas_notify_lldd_dev_gone(dev);
	if (!dev->parent)
		dev->port->port_dev = NULL;
	else
		list_del_init(&dev->siblings);

	spin_lock_irq(&port->dev_list_lock);
	list_del_init(&dev->dev_list_node);
	if (dev_is_sata(dev))
		sas_ata_end_eh(dev->sata_dev.ap);
	spin_unlock_irq(&port->dev_list_lock);

	spin_lock_irq(&ha->lock);
	if (dev->dev_type == SAS_END_DEVICE &&
	    !list_empty(&dev->ssp_dev.eh_list_node)) {
		list_del_init(&dev->ssp_dev.eh_list_node);
		ha->eh_active--;
	}
	spin_unlock_irq(&ha->lock);

	sas_put_device(dev);
}