static int __init packet_init(void)
{
	int rc = proto_register(&packet_proto, 0);

	if (rc != 0)
		goto out;

	sock_register(&packet_family_ops);
	register_pernet_subsys(&packet_net_ops);
	register_netdevice_notifier(&packet_netdev_notifier);
out:
	return rc;
}