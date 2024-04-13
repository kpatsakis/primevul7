static int is_cmd_rcvr_exclusive(struct ipmi_smi *intf,
				 unsigned char netfn,
				 unsigned char cmd,
				 unsigned int  chans)
{
	struct cmd_rcvr *rcvr;

	list_for_each_entry_rcu(rcvr, &intf->cmd_rcvrs, link) {
		if ((rcvr->netfn == netfn) && (rcvr->cmd == cmd)
					&& (rcvr->chans & chans))
			return 0;
	}
	return 1;
}