int nsim_bpf_dev_init(struct nsim_dev *nsim_dev)
{
	int err;

	INIT_LIST_HEAD(&nsim_dev->bpf_bound_progs);
	INIT_LIST_HEAD(&nsim_dev->bpf_bound_maps);

	nsim_dev->ddir_bpf_bound_progs = debugfs_create_dir("bpf_bound_progs",
							    nsim_dev->ddir);
	if (IS_ERR(nsim_dev->ddir_bpf_bound_progs))
		return PTR_ERR(nsim_dev->ddir_bpf_bound_progs);

	nsim_dev->bpf_dev = bpf_offload_dev_create(&nsim_bpf_dev_ops, nsim_dev);
	err = PTR_ERR_OR_ZERO(nsim_dev->bpf_dev);
	if (err)
		return err;

	nsim_dev->bpf_bind_accept = true;
	debugfs_create_bool("bpf_bind_accept", 0600, nsim_dev->ddir,
			    &nsim_dev->bpf_bind_accept);
	debugfs_create_u32("bpf_bind_verifier_delay", 0600, nsim_dev->ddir,
			   &nsim_dev->bpf_bind_verifier_delay);
	nsim_dev->bpf_bind_verifier_accept = true;
	debugfs_create_bool("bpf_bind_verifier_accept", 0600, nsim_dev->ddir,
			    &nsim_dev->bpf_bind_verifier_accept);
	return 0;
}