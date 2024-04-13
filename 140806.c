static void nf_conntrack_pernet_exit(struct list_head *net_exit_list)
{
	struct net *net;

	list_for_each_entry(net, net_exit_list, exit_list)
		nf_conntrack_fini_net(net);

	nf_conntrack_cleanup_net_list(net_exit_list);
}