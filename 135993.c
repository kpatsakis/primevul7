int nsim_bpf_disable_tc(struct netdevsim *ns)
{
	if (ns->bpf_offloaded && !nsim_xdp_offload_active(ns))
		return -EBUSY;
	return 0;
}