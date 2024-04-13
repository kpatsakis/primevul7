static void nsim_prog_set_loaded(struct bpf_prog *prog, bool loaded)
{
	struct nsim_bpf_bound_prog *state;

	if (!prog || !prog->aux->offload)
		return;

	state = prog->aux->offload->dev_priv;
	state->is_loaded = loaded;
}