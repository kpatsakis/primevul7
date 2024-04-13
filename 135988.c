int nsim_bpf(struct net_device *dev, struct netdev_bpf *bpf)
{
	struct netdevsim *ns = netdev_priv(dev);
	int err;

	ASSERT_RTNL();

	switch (bpf->command) {
	case XDP_SETUP_PROG:
		err = nsim_setup_prog_checks(ns, bpf);
		if (err)
			return err;

		return nsim_xdp_set_prog(ns, bpf, &ns->xdp);
	case XDP_SETUP_PROG_HW:
		err = nsim_setup_prog_hw_checks(ns, bpf);
		if (err)
			return err;

		return nsim_xdp_set_prog(ns, bpf, &ns->xdp_hw);
	case BPF_OFFLOAD_MAP_ALLOC:
		if (!ns->bpf_map_accept)
			return -EOPNOTSUPP;

		return nsim_bpf_map_alloc(ns, bpf->offmap);
	case BPF_OFFLOAD_MAP_FREE:
		nsim_bpf_map_free(bpf->offmap);
		return 0;
	default:
		return -EINVAL;
	}
}