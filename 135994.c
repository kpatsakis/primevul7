int nsim_bpf_init(struct netdevsim *ns)
{
	struct dentry *ddir = ns->nsim_dev_port->ddir;
	int err;

	err = bpf_offload_dev_netdev_register(ns->nsim_dev->bpf_dev,
					      ns->netdev);
	if (err)
		return err;

	debugfs_create_u32("bpf_offloaded_id", 0400, ddir,
			   &ns->bpf_offloaded_id);

	ns->bpf_tc_accept = true;
	debugfs_create_bool("bpf_tc_accept", 0600, ddir,
			    &ns->bpf_tc_accept);
	debugfs_create_bool("bpf_tc_non_bound_accept", 0600, ddir,
			    &ns->bpf_tc_non_bound_accept);
	ns->bpf_xdpdrv_accept = true;
	debugfs_create_bool("bpf_xdpdrv_accept", 0600, ddir,
			    &ns->bpf_xdpdrv_accept);
	ns->bpf_xdpoffload_accept = true;
	debugfs_create_bool("bpf_xdpoffload_accept", 0600, ddir,
			    &ns->bpf_xdpoffload_accept);

	ns->bpf_map_accept = true;
	debugfs_create_bool("bpf_map_accept", 0600, ddir,
			    &ns->bpf_map_accept);

	return 0;
}