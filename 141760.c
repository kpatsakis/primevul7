static int __init xfrm_user_init(void)
{
	int rv;

	printk(KERN_INFO "Initializing XFRM netlink socket\n");

	rv = register_pernet_subsys(&xfrm_user_net_ops);
	if (rv < 0)
		return rv;
	rv = xfrm_register_km(&netlink_mgr);
	if (rv < 0)
		unregister_pernet_subsys(&xfrm_user_net_ops);
	return rv;
}