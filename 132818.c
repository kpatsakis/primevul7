static void cleanup_smi_msgs(struct ipmi_smi *intf)
{
	int              i;
	struct seq_table *ent;
	struct ipmi_smi_msg *msg;
	struct list_head *entry;
	struct list_head tmplist;

	/* Clear out our transmit queues and hold the messages. */
	INIT_LIST_HEAD(&tmplist);
	list_splice_tail(&intf->hp_xmit_msgs, &tmplist);
	list_splice_tail(&intf->xmit_msgs, &tmplist);

	/* Current message first, to preserve order */
	while (intf->curr_msg && !list_empty(&intf->waiting_rcv_msgs)) {
		/* Wait for the message to clear out. */
		schedule_timeout(1);
	}

	/* No need for locks, the interface is down. */

	/*
	 * Return errors for all pending messages in queue and in the
	 * tables waiting for remote responses.
	 */
	while (!list_empty(&tmplist)) {
		entry = tmplist.next;
		list_del(entry);
		msg = list_entry(entry, struct ipmi_smi_msg, link);
		deliver_smi_err_response(intf, msg, IPMI_ERR_UNSPECIFIED);
	}

	for (i = 0; i < IPMI_IPMB_NUM_SEQ; i++) {
		ent = &intf->seq_table[i];
		if (!ent->inuse)
			continue;
		deliver_err_response(intf, ent->recv_msg, IPMI_ERR_UNSPECIFIED);
	}
}