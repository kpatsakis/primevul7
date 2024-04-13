static void free_smi_msg_list(struct list_head *q)
{
	struct ipmi_smi_msg *msg, *msg2;

	list_for_each_entry_safe(msg, msg2, q, link) {
		list_del(&msg->link);
		ipmi_free_smi_msg(msg);
	}
}