
static int dev_xdp_install(struct net_device *dev, bpf_op_t bpf_op,
			   struct netlink_ext_ack *extack, u32 flags,
			   struct bpf_prog *prog)
{
	struct netdev_bpf xdp;

	memset(&xdp, 0, sizeof(xdp));
	if (flags & XDP_FLAGS_HW_MODE)
		xdp.command = XDP_SETUP_PROG_HW;
	else
		xdp.command = XDP_SETUP_PROG;
	xdp.extack = extack;
	xdp.flags = flags;
	xdp.prog = prog;

	return bpf_op(dev, &xdp);