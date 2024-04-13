int ipmi_get_smi_info(int if_num, struct ipmi_smi_info *data)
{
	int rv, index;
	struct ipmi_smi *intf;

	index = srcu_read_lock(&ipmi_interfaces_srcu);
	list_for_each_entry_rcu(intf, &ipmi_interfaces, link) {
		if (intf->intf_num == if_num)
			goto found;
	}
	srcu_read_unlock(&ipmi_interfaces_srcu, index);

	/* Not found, return an error */
	return -EINVAL;

found:
	if (!intf->handlers->get_smi_info)
		rv = -ENOTTY;
	else
		rv = intf->handlers->get_smi_info(intf->send_info, data);
	srcu_read_unlock(&ipmi_interfaces_srcu, index);

	return rv;
}