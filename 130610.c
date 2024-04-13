void sas_unregister_domain_devices(struct asd_sas_port *port, int gone)
{
	struct domain_device *dev, *n;

	list_for_each_entry_safe_reverse(dev, n, &port->dev_list, dev_list_node) {
		if (gone)
			set_bit(SAS_DEV_GONE, &dev->state);
		sas_unregister_dev(port, dev);
	}

	list_for_each_entry_safe(dev, n, &port->disco_list, disco_list_node)
		sas_unregister_dev(port, dev);

	port->port->rphy = NULL;

}