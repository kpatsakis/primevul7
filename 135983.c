static int nsim_bpf_create_prog(struct nsim_dev *nsim_dev,
				struct bpf_prog *prog)
{
	struct nsim_bpf_bound_prog *state;
	char name[16];
	int ret;

	state = kzalloc(sizeof(*state), GFP_KERNEL);
	if (!state)
		return -ENOMEM;

	state->nsim_dev = nsim_dev;
	state->prog = prog;
	state->state = "verify";

	/* Program id is not populated yet when we create the state. */
	sprintf(name, "%u", nsim_dev->prog_id_gen++);
	state->ddir = debugfs_create_dir(name, nsim_dev->ddir_bpf_bound_progs);
	if (IS_ERR(state->ddir)) {
		ret = PTR_ERR(state->ddir);
		kfree(state);
		return ret;
	}

	debugfs_create_u32("id", 0400, state->ddir, &prog->aux->id);
	debugfs_create_file("state", 0400, state->ddir,
			    &state->state, &nsim_bpf_string_fops);
	debugfs_create_bool("loaded", 0400, state->ddir, &state->is_loaded);

	list_add_tail(&state->l, &nsim_dev->bpf_bound_progs);

	prog->aux->offload->dev_priv = state;

	return 0;
}