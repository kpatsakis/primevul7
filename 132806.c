static int i_ipmi_req_ipmb(struct ipmi_smi        *intf,
			   struct ipmi_addr       *addr,
			   long                   msgid,
			   struct kernel_ipmi_msg *msg,
			   struct ipmi_smi_msg    *smi_msg,
			   struct ipmi_recv_msg   *recv_msg,
			   unsigned char          source_address,
			   unsigned char          source_lun,
			   int                    retries,
			   unsigned int           retry_time_ms)
{
	struct ipmi_ipmb_addr *ipmb_addr;
	unsigned char ipmb_seq;
	long seqid;
	int broadcast = 0;
	struct ipmi_channel *chans;
	int rv = 0;

	if (addr->channel >= IPMI_MAX_CHANNELS) {
		ipmi_inc_stat(intf, sent_invalid_commands);
		return -EINVAL;
	}

	chans = READ_ONCE(intf->channel_list)->c;

	if (chans[addr->channel].medium != IPMI_CHANNEL_MEDIUM_IPMB) {
		ipmi_inc_stat(intf, sent_invalid_commands);
		return -EINVAL;
	}

	if (addr->addr_type == IPMI_IPMB_BROADCAST_ADDR_TYPE) {
		/*
		 * Broadcasts add a zero at the beginning of the
		 * message, but otherwise is the same as an IPMB
		 * address.
		 */
		addr->addr_type = IPMI_IPMB_ADDR_TYPE;
		broadcast = 1;
		retries = 0; /* Don't retry broadcasts. */
	}

	/*
	 * 9 for the header and 1 for the checksum, plus
	 * possibly one for the broadcast.
	 */
	if ((msg->data_len + 10 + broadcast) > IPMI_MAX_MSG_LENGTH) {
		ipmi_inc_stat(intf, sent_invalid_commands);
		return -EMSGSIZE;
	}

	ipmb_addr = (struct ipmi_ipmb_addr *) addr;
	if (ipmb_addr->lun > 3) {
		ipmi_inc_stat(intf, sent_invalid_commands);
		return -EINVAL;
	}

	memcpy(&recv_msg->addr, ipmb_addr, sizeof(*ipmb_addr));

	if (recv_msg->msg.netfn & 0x1) {
		/*
		 * It's a response, so use the user's sequence
		 * from msgid.
		 */
		ipmi_inc_stat(intf, sent_ipmb_responses);
		format_ipmb_msg(smi_msg, msg, ipmb_addr, msgid,
				msgid, broadcast,
				source_address, source_lun);

		/*
		 * Save the receive message so we can use it
		 * to deliver the response.
		 */
		smi_msg->user_data = recv_msg;
	} else {
		/* It's a command, so get a sequence for it. */
		unsigned long flags;

		spin_lock_irqsave(&intf->seq_lock, flags);

		if (is_maintenance_mode_cmd(msg))
			intf->ipmb_maintenance_mode_timeout =
				maintenance_mode_timeout_ms;

		if (intf->ipmb_maintenance_mode_timeout && retry_time_ms == 0)
			/* Different default in maintenance mode */
			retry_time_ms = default_maintenance_retry_ms;

		/*
		 * Create a sequence number with a 1 second
		 * timeout and 4 retries.
		 */
		rv = intf_next_seq(intf,
				   recv_msg,
				   retry_time_ms,
				   retries,
				   broadcast,
				   &ipmb_seq,
				   &seqid);
		if (rv)
			/*
			 * We have used up all the sequence numbers,
			 * probably, so abort.
			 */
			goto out_err;

		ipmi_inc_stat(intf, sent_ipmb_commands);

		/*
		 * Store the sequence number in the message,
		 * so that when the send message response
		 * comes back we can start the timer.
		 */
		format_ipmb_msg(smi_msg, msg, ipmb_addr,
				STORE_SEQ_IN_MSGID(ipmb_seq, seqid),
				ipmb_seq, broadcast,
				source_address, source_lun);

		/*
		 * Copy the message into the recv message data, so we
		 * can retransmit it later if necessary.
		 */
		memcpy(recv_msg->msg_data, smi_msg->data,
		       smi_msg->data_size);
		recv_msg->msg.data = recv_msg->msg_data;
		recv_msg->msg.data_len = smi_msg->data_size;

		/*
		 * We don't unlock until here, because we need
		 * to copy the completed message into the
		 * recv_msg before we release the lock.
		 * Otherwise, race conditions may bite us.  I
		 * know that's pretty paranoid, but I prefer
		 * to be correct.
		 */
out_err:
		spin_unlock_irqrestore(&intf->seq_lock, flags);
	}

	return rv;
}