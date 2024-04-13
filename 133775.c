
static void dev_xdp_uninstall(struct net_device *dev)
{
	struct netdev_bpf xdp;
	bpf_op_t ndo_bpf;

	/* Remove generic XDP */
	WARN_ON(dev_xdp_install(dev, generic_xdp_install, NULL, 0, NULL));

	/* Remove from the driver */
	ndo_bpf = dev->netdev_ops->ndo_bpf;
	if (!ndo_bpf)
		return;

	memset(&xdp, 0, sizeof(xdp));
	xdp.command = XDP_QUERY_PROG;
	WARN_ON(ndo_bpf(dev, &xdp));
	if (xdp.prog_id)
		WARN_ON(dev_xdp_install(dev, ndo_bpf, NULL, xdp.prog_flags,
					NULL));

	/* Remove HW offload */
	memset(&xdp, 0, sizeof(xdp));
	xdp.command = XDP_QUERY_PROG_HW;
	if (!ndo_bpf(dev, &xdp) && xdp.prog_id)
		WARN_ON(dev_xdp_install(dev, ndo_bpf, NULL, xdp.prog_flags,
					NULL));