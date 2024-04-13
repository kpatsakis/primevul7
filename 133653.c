
u32 __dev_xdp_query(struct net_device *dev, bpf_op_t bpf_op,
		    enum bpf_netdev_command cmd)
{
	struct netdev_bpf xdp;

	if (!bpf_op)
		return 0;

	memset(&xdp, 0, sizeof(xdp));
	xdp.command = cmd;

	/* Query must always succeed. */
	WARN_ON(bpf_op(dev, &xdp) < 0 && cmd == XDP_QUERY_PROG);

	return xdp.prog_id;