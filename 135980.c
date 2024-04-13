static int nsim_xdp_offload_prog(struct netdevsim *ns, struct netdev_bpf *bpf)
{
	if (!nsim_xdp_offload_active(ns) && !bpf->prog)
		return 0;
	if (!nsim_xdp_offload_active(ns) && bpf->prog && ns->bpf_offloaded) {
		NSIM_EA(bpf->extack, "TC program is already loaded");
		return -EBUSY;
	}

	return nsim_bpf_offload(ns, bpf->prog, nsim_xdp_offload_active(ns));
}