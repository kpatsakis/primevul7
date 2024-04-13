void nsim_bpf_dev_exit(struct nsim_dev *nsim_dev)
{
	WARN_ON(!list_empty(&nsim_dev->bpf_bound_progs));
	WARN_ON(!list_empty(&nsim_dev->bpf_bound_maps));
	bpf_offload_dev_destroy(nsim_dev->bpf_dev);
}