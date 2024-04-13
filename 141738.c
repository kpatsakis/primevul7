static void __exit xfrm_user_exit(void)
{
	xfrm_unregister_km(&netlink_mgr);
	unregister_pernet_subsys(&xfrm_user_net_ops);
}