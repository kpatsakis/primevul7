nsim_setup_prog_hw_checks(struct netdevsim *ns, struct netdev_bpf *bpf)
{
	struct nsim_bpf_bound_prog *state;

	if (!bpf->prog)
		return 0;

	if (!bpf->prog->aux->offload) {
		NSIM_EA(bpf->extack, "xdpoffload of non-bound program");
		return -EINVAL;
	}
	if (!bpf_offload_dev_match(bpf->prog, ns->netdev)) {
		NSIM_EA(bpf->extack, "program bound to different dev");
		return -EINVAL;
	}

	state = bpf->prog->aux->offload->dev_priv;
	if (WARN_ON(strcmp(state->state, "xlated"))) {
		NSIM_EA(bpf->extack, "offloading program in bad state");
		return -EINVAL;
	}
	return 0;
}