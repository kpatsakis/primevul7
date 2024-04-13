nsim_xdp_set_prog(struct netdevsim *ns, struct netdev_bpf *bpf,
		  struct xdp_attachment_info *xdp)
{
	int err;

	if (bpf->command == XDP_SETUP_PROG && !ns->bpf_xdpdrv_accept) {
		NSIM_EA(bpf->extack, "driver XDP disabled in DebugFS");
		return -EOPNOTSUPP;
	}
	if (bpf->command == XDP_SETUP_PROG_HW && !ns->bpf_xdpoffload_accept) {
		NSIM_EA(bpf->extack, "XDP offload disabled in DebugFS");
		return -EOPNOTSUPP;
	}

	if (bpf->command == XDP_SETUP_PROG_HW) {
		err = nsim_xdp_offload_prog(ns, bpf);
		if (err)
			return err;
	}

	xdp_attachment_setup(xdp, bpf);

	return 0;
}