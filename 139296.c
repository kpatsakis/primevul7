static void __exit af_unix_exit(void)
{
	sock_unregister(PF_UNIX);
	proto_unregister(&unix_proto);
	unregister_pernet_subsys(&unix_net_ops);
}