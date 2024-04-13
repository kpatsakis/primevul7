static int nf_conntrack_pernet_init(struct net *net)
{
	int ret;

	net->ct.sysctl_checksum = 1;

	ret = nf_conntrack_standalone_init_sysctl(net);
	if (ret < 0)
		return ret;

	ret = nf_conntrack_standalone_init_proc(net);
	if (ret < 0)
		goto out_proc;

	ret = nf_conntrack_init_net(net);
	if (ret < 0)
		goto out_init_net;

	if (enable_hooks) {
		ret = nf_ct_netns_get(net, NFPROTO_INET);
		if (ret < 0)
			goto out_hooks;
	}

	return 0;

out_hooks:
	nf_conntrack_cleanup_net(net);
out_init_net:
	nf_conntrack_standalone_fini_proc(net);
out_proc:
	nf_conntrack_standalone_fini_sysctl(net);
	return ret;
}