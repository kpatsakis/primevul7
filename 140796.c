static void nf_conntrack_standalone_init_sctp_sysctl(struct net *net,
						     struct ctl_table *table)
{
#ifdef CONFIG_NF_CT_PROTO_SCTP
	struct nf_sctp_net *sn = nf_sctp_pernet(net);

#define XASSIGN(XNAME, sn) \
	table[NF_SYSCTL_CT_PROTO_TIMEOUT_SCTP_ ## XNAME].data = \
			&(sn)->timeouts[SCTP_CONNTRACK_ ## XNAME]

	XASSIGN(CLOSED, sn);
	XASSIGN(COOKIE_WAIT, sn);
	XASSIGN(COOKIE_ECHOED, sn);
	XASSIGN(ESTABLISHED, sn);
	XASSIGN(SHUTDOWN_SENT, sn);
	XASSIGN(SHUTDOWN_RECD, sn);
	XASSIGN(SHUTDOWN_ACK_SENT, sn);
	XASSIGN(HEARTBEAT_SENT, sn);
	XASSIGN(HEARTBEAT_ACKED, sn);
#undef XASSIGN
#endif
}