static int __scan_channels(struct ipmi_smi *intf, struct ipmi_device_id *id)
{
	int rv;

	if (ipmi_version_major(id) > 1
			|| (ipmi_version_major(id) == 1
			    && ipmi_version_minor(id) >= 5)) {
		unsigned int set;

		/*
		 * Start scanning the channels to see what is
		 * available.
		 */
		set = !intf->curr_working_cset;
		intf->curr_working_cset = set;
		memset(&intf->wchannels[set], 0,
		       sizeof(struct ipmi_channel_set));

		intf->null_user_handler = channel_handler;
		intf->curr_channel = 0;
		rv = send_channel_info_cmd(intf, 0);
		if (rv) {
			dev_warn(intf->si_dev,
				 "Error sending channel information for channel 0, %d\n",
				 rv);
			return -EIO;
		}

		/* Wait for the channel info to be read. */
		wait_event(intf->waitq, intf->channels_ready);
		intf->null_user_handler = NULL;
	} else {
		unsigned int set = intf->curr_working_cset;

		/* Assume a single IPMB channel at zero. */
		intf->wchannels[set].c[0].medium = IPMI_CHANNEL_MEDIUM_IPMB;
		intf->wchannels[set].c[0].protocol = IPMI_CHANNEL_PROTOCOL_IPMB;
		intf->channel_list = intf->wchannels + set;
		intf->channels_ready = true;
	}

	return 0;
}