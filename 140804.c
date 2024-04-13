static void nf_conntrack_standalone_init_dccp_sysctl(struct net *net,
						     struct ctl_table *table)
{
#ifdef CONFIG_NF_CT_PROTO_DCCP
	struct nf_dccp_net *dn = nf_dccp_pernet(net);

#define XASSIGN(XNAME, dn) \
	table[NF_SYSCTL_CT_PROTO_TIMEOUT_DCCP_ ## XNAME].data = \
			&(dn)->dccp_timeout[CT_DCCP_ ## XNAME]

	XASSIGN(REQUEST, dn);
	XASSIGN(RESPOND, dn);
	XASSIGN(PARTOPEN, dn);
	XASSIGN(OPEN, dn);
	XASSIGN(CLOSEREQ, dn);
	XASSIGN(CLOSING, dn);
	XASSIGN(TIMEWAIT, dn);
#undef XASSIGN

	table[NF_SYSCTL_CT_PROTO_DCCP_LOOSE].data = &dn->dccp_loose;
#endif
}