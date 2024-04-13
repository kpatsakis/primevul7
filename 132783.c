static struct cmd_rcvr *find_cmd_rcvr(struct ipmi_smi *intf,
				      unsigned char netfn,
				      unsigned char cmd,
				      unsigned char chan)
{
	struct cmd_rcvr *rcvr;

	list_for_each_entry_rcu(rcvr, &intf->cmd_rcvrs, link) {
		if ((rcvr->netfn == netfn) && (rcvr->cmd == cmd)
					&& (rcvr->chans & (1 << chan)))
			return rcvr;
	}
	return NULL;
}