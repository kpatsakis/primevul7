static void __exit esp6_fini(void)
{
	if (xfrm6_protocol_deregister(&esp6_protocol, IPPROTO_ESP) < 0)
		pr_info("%s: can't remove protocol\n", __func__);
	xfrm_unregister_type(&esp6_type, AF_INET6);
}