channel_handler(struct ipmi_smi *intf, struct ipmi_recv_msg *msg)
{
	int rv = 0;
	int ch;
	unsigned int set = intf->curr_working_cset;
	struct ipmi_channel *chans;

	if ((msg->addr.addr_type == IPMI_SYSTEM_INTERFACE_ADDR_TYPE)
	    && (msg->msg.netfn == IPMI_NETFN_APP_RESPONSE)
	    && (msg->msg.cmd == IPMI_GET_CHANNEL_INFO_CMD)) {
		/* It's the one we want */
		if (msg->msg.data[0] != 0) {
			/* Got an error from the channel, just go on. */

			if (msg->msg.data[0] == IPMI_INVALID_COMMAND_ERR) {
				/*
				 * If the MC does not support this
				 * command, that is legal.  We just
				 * assume it has one IPMB at channel
				 * zero.
				 */
				intf->wchannels[set].c[0].medium
					= IPMI_CHANNEL_MEDIUM_IPMB;
				intf->wchannels[set].c[0].protocol
					= IPMI_CHANNEL_PROTOCOL_IPMB;

				intf->channel_list = intf->wchannels + set;
				intf->channels_ready = true;
				wake_up(&intf->waitq);
				goto out;
			}
			goto next_channel;
		}
		if (msg->msg.data_len < 4) {
			/* Message not big enough, just go on. */
			goto next_channel;
		}
		ch = intf->curr_channel;
		chans = intf->wchannels[set].c;
		chans[ch].medium = msg->msg.data[2] & 0x7f;
		chans[ch].protocol = msg->msg.data[3] & 0x1f;

 next_channel:
		intf->curr_channel++;
		if (intf->curr_channel >= IPMI_MAX_CHANNELS) {
			intf->channel_list = intf->wchannels + set;
			intf->channels_ready = true;
			wake_up(&intf->waitq);
		} else {
			intf->channel_list = intf->wchannels + set;
			intf->channels_ready = true;
			rv = send_channel_info_cmd(intf, intf->curr_channel);
		}

		if (rv) {
			/* Got an error somehow, just give up. */
			dev_warn(intf->si_dev,
				 "Error sending channel information for channel %d: %d\n",
				 intf->curr_channel, rv);

			intf->channel_list = intf->wchannels + set;
			intf->channels_ready = true;
			wake_up(&intf->waitq);
		}
	}
 out:
	return;
}