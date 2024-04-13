static int nsim_setup_prog_checks(struct netdevsim *ns, struct netdev_bpf *bpf)
{
	if (bpf->prog && bpf->prog->aux->offload) {
		NSIM_EA(bpf->extack, "attempt to load offloaded prog to drv");
		return -EINVAL;
	}
	if (ns->netdev->mtu > NSIM_XDP_MAX_MTU) {
		NSIM_EA(bpf->extack, "MTU too large w/ XDP enabled");
		return -EINVAL;
	}
	return 0;
}