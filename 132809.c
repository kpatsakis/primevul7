static int intf_find_seq(struct ipmi_smi      *intf,
			 unsigned char        seq,
			 short                channel,
			 unsigned char        cmd,
			 unsigned char        netfn,
			 struct ipmi_addr     *addr,
			 struct ipmi_recv_msg **recv_msg)
{
	int           rv = -ENODEV;
	unsigned long flags;

	if (seq >= IPMI_IPMB_NUM_SEQ)
		return -EINVAL;

	spin_lock_irqsave(&intf->seq_lock, flags);
	if (intf->seq_table[seq].inuse) {
		struct ipmi_recv_msg *msg = intf->seq_table[seq].recv_msg;

		if ((msg->addr.channel == channel) && (msg->msg.cmd == cmd)
				&& (msg->msg.netfn == netfn)
				&& (ipmi_addr_equal(addr, &msg->addr))) {
			*recv_msg = msg;
			intf->seq_table[seq].inuse = 0;
			smi_remove_watch(intf, IPMI_WATCH_MASK_CHECK_MESSAGES);
			rv = 0;
		}
	}
	spin_unlock_irqrestore(&intf->seq_lock, flags);

	return rv;
}