static void nf_conntrack_standalone_init_gre_sysctl(struct net *net,
						    struct ctl_table *table)
{
#ifdef CONFIG_NF_CT_PROTO_GRE
	struct nf_gre_net *gn = nf_gre_pernet(net);

	table[NF_SYSCTL_CT_PROTO_TIMEOUT_GRE].data = &gn->timeouts[GRE_CT_UNREPLIED];
	table[NF_SYSCTL_CT_PROTO_TIMEOUT_GRE_STREAM].data = &gn->timeouts[GRE_CT_REPLIED];
#endif
}