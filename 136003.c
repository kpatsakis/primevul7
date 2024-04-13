void nsim_bpf_uninit(struct netdevsim *ns)
{
	WARN_ON(ns->xdp.prog);
	WARN_ON(ns->xdp_hw.prog);
	WARN_ON(ns->bpf_offloaded);
	bpf_offload_dev_netdev_unregister(ns->nsim_dev->bpf_dev, ns->netdev);
}