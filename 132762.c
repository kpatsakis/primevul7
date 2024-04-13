static struct ipmi_smi_msg *smi_add_send_msg(struct ipmi_smi *intf,
					     struct ipmi_smi_msg *smi_msg,
					     int priority)
{
	if (intf->curr_msg) {
		if (priority > 0)
			list_add_tail(&smi_msg->link, &intf->hp_xmit_msgs);
		else
			list_add_tail(&smi_msg->link, &intf->xmit_msgs);
		smi_msg = NULL;
	} else {
		intf->curr_msg = smi_msg;
	}

	return smi_msg;
}