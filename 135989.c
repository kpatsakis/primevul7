static void nsim_bpf_destroy_prog(struct bpf_prog *prog)
{
	struct nsim_bpf_bound_prog *state;

	state = prog->aux->offload->dev_priv;
	WARN(state->is_loaded,
	     "offload state destroyed while program still bound");
	debugfs_remove_recursive(state->ddir);
	list_del(&state->l);
	kfree(state);
}