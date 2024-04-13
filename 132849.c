static void clean_up_interface_data(struct ipmi_smi *intf)
{
	int              i;
	struct cmd_rcvr  *rcvr, *rcvr2;
	struct list_head list;

	tasklet_kill(&intf->recv_tasklet);

	free_smi_msg_list(&intf->waiting_rcv_msgs);
	free_recv_msg_list(&intf->waiting_events);

	/*
	 * Wholesale remove all the entries from the list in the
	 * interface and wait for RCU to know that none are in use.
	 */
	mutex_lock(&intf->cmd_rcvrs_mutex);
	INIT_LIST_HEAD(&list);
	list_splice_init_rcu(&intf->cmd_rcvrs, &list, synchronize_rcu);
	mutex_unlock(&intf->cmd_rcvrs_mutex);

	list_for_each_entry_safe(rcvr, rcvr2, &list, link)
		kfree(rcvr);

	for (i = 0; i < IPMI_IPMB_NUM_SEQ; i++) {
		if ((intf->seq_table[i].inuse)
					&& (intf->seq_table[i].recv_msg))
			ipmi_free_recv_msg(intf->seq_table[i].recv_msg);
	}
}