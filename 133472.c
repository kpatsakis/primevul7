 */
int dev_change_xdp_fd(struct net_device *dev, struct netlink_ext_ack *extack,
		      int fd, u32 flags)
{
	const struct net_device_ops *ops = dev->netdev_ops;
	enum bpf_netdev_command query;
	struct bpf_prog *prog = NULL;
	bpf_op_t bpf_op, bpf_chk;
	bool offload;
	int err;

	ASSERT_RTNL();

	offload = flags & XDP_FLAGS_HW_MODE;
	query = offload ? XDP_QUERY_PROG_HW : XDP_QUERY_PROG;

	bpf_op = bpf_chk = ops->ndo_bpf;
	if (!bpf_op && (flags & (XDP_FLAGS_DRV_MODE | XDP_FLAGS_HW_MODE))) {
		NL_SET_ERR_MSG(extack, "underlying driver does not support XDP in native mode");
		return -EOPNOTSUPP;
	}
	if (!bpf_op || (flags & XDP_FLAGS_SKB_MODE))
		bpf_op = generic_xdp_install;
	if (bpf_op == bpf_chk)
		bpf_chk = generic_xdp_install;

	if (fd >= 0) {
		if (!offload && __dev_xdp_query(dev, bpf_chk, XDP_QUERY_PROG)) {
			NL_SET_ERR_MSG(extack, "native and generic XDP can't be active at the same time");
			return -EEXIST;
		}
		if ((flags & XDP_FLAGS_UPDATE_IF_NOEXIST) &&
		    __dev_xdp_query(dev, bpf_op, query)) {
			NL_SET_ERR_MSG(extack, "XDP program already attached");
			return -EBUSY;
		}

		prog = bpf_prog_get_type_dev(fd, BPF_PROG_TYPE_XDP,
					     bpf_op == ops->ndo_bpf);
		if (IS_ERR(prog))
			return PTR_ERR(prog);

		if (!offload && bpf_prog_is_dev_bound(prog->aux)) {
			NL_SET_ERR_MSG(extack, "using device-bound program without HW_MODE flag is not supported");
			bpf_prog_put(prog);
			return -EINVAL;
		}
	}

	err = dev_xdp_install(dev, bpf_op, extack, flags, prog);
	if (err < 0 && prog)
		bpf_prog_put(prog);

	return err;