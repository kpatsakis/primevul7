static void nf_conntrack_standalone_init_tcp_sysctl(struct net *net,
						    struct ctl_table *table)
{
	struct nf_tcp_net *tn = nf_tcp_pernet(net);

#define XASSIGN(XNAME, tn) \
	table[NF_SYSCTL_CT_PROTO_TIMEOUT_TCP_ ## XNAME].data = \
			&(tn)->timeouts[TCP_CONNTRACK_ ## XNAME]

	XASSIGN(SYN_SENT, tn);
	XASSIGN(SYN_RECV, tn);
	XASSIGN(ESTABLISHED, tn);
	XASSIGN(FIN_WAIT, tn);
	XASSIGN(CLOSE_WAIT, tn);
	XASSIGN(LAST_ACK, tn);
	XASSIGN(TIME_WAIT, tn);
	XASSIGN(CLOSE, tn);
	XASSIGN(RETRANS, tn);
	XASSIGN(UNACK, tn);
#undef XASSIGN
#define XASSIGN(XNAME, rval) \
	table[NF_SYSCTL_CT_PROTO_TCP_ ## XNAME].data = (rval)

	XASSIGN(LOOSE, &tn->tcp_loose);
	XASSIGN(LIBERAL, &tn->tcp_be_liberal);
	XASSIGN(MAX_RETRANS, &tn->tcp_max_retrans);
#undef XASSIGN
}