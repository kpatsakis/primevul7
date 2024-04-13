static void nf_conntrack_standalone_fini_sysctl(struct net *net)
{
	struct nf_conntrack_net *cnet = net_generic(net, nf_conntrack_net_id);
	struct ctl_table *table;

	table = cnet->sysctl_header->ctl_table_arg;
	unregister_net_sysctl_table(cnet->sysctl_header);
	kfree(table);
}