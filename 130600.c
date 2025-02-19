void sas_unregister_dev(struct asd_sas_port *port, struct domain_device *dev)
{
	if (!test_bit(SAS_DEV_DESTROY, &dev->state) &&
	    !list_empty(&dev->disco_list_node)) {
		/* this rphy never saw sas_rphy_add */
		list_del_init(&dev->disco_list_node);
		sas_rphy_free(dev->rphy);
		sas_unregister_common_dev(port, dev);
		return;
	}

	if (!test_and_set_bit(SAS_DEV_DESTROY, &dev->state)) {
		sas_rphy_unlink(dev->rphy);
		list_move_tail(&dev->disco_list_node, &port->destroy_list);
	}
}