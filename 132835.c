static int i_ipmi_req_lan(struct ipmi_smi        *intf,
			  struct ipmi_addr       *addr,
			  long                   msgid,
			  struct kernel_ipmi_msg *msg,
			  struct ipmi_smi_msg    *smi_msg,
			  struct ipmi_recv_msg   *recv_msg,
			  unsigned char          source_lun,
			  int                    retries,
			  unsigned int           retry_time_ms)
{
	struct ipmi_lan_addr  *lan_addr;
	unsigned char ipmb_seq;
	long seqid;
	struct ipmi_channel *chans;
	int rv = 0;

	if (addr->channel >= IPMI_MAX_CHANNELS) {
		ipmi_inc_stat(intf, sent_invalid_commands);
		return -EINVAL;
	}

	chans = READ_ONCE(intf->channel_list)->c;

	if ((chans[addr->channel].medium
				!= IPMI_CHANNEL_MEDIUM_8023LAN)
			&& (chans[addr->channel].medium
			    != IPMI_CHANNEL_MEDIUM_ASYNC)) {
		ipmi_inc_stat(intf, sent_invalid_commands);
		return -EINVAL;
	}

	/* 11 for the header and 1 for the checksum. */
	if ((msg->data_len + 12) > IPMI_MAX_MSG_LENGTH) {
		ipmi_inc_stat(intf, sent_invalid_commands);
		return -EMSGSIZE;
	}

	lan_addr = (struct ipmi_lan_addr *) addr;
	if (lan_addr->lun > 3) {
		ipmi_inc_stat(intf, sent_invalid_commands);
		return -EINVAL;
	}

	memcpy(&recv_msg->addr, lan_addr, sizeof(*lan_addr));

	if (recv_msg->msg.netfn & 0x1) {
		/*
		 * It's a response, so use the user's sequence
		 * from msgid.
		 */
		ipmi_inc_stat(intf, sent_lan_responses);
		format_lan_msg(smi_msg, msg, lan_addr, msgid,
			       msgid, source_lun);

		/*
		 * Save the receive message so we can use it
		 * to deliver the response.
		 */
		smi_msg->user_data = recv_msg;
	} else {
		/* It's a command, so get a sequence for it. */
		unsigned long flags;

		spin_lock_irqsave(&intf->seq_lock, flags);

		/*
		 * Create a sequence number with a 1 second
		 * timeout and 4 retries.
		 */
		rv = intf_next_seq(intf,
				   recv_msg,
				   retry_time_ms,
				   retries,
				   0,
				   &ipmb_seq,
				   &seqid);
		if (rv)
			/*
			 * We have used up all the sequence numbers,
			 * probably, so abort.
			 */
			goto out_err;

		ipmi_inc_stat(intf, sent_lan_commands);

		/*
		 * Store the sequence number in the message,
		 * so that when the send message response
		 * comes back we can start the timer.
		 */
		format_lan_msg(smi_msg, msg, lan_addr,
			       STORE_SEQ_IN_MSGID(ipmb_seq, seqid),
			       ipmb_seq, source_lun);

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