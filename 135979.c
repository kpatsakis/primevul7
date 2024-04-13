static int nsim_bpf_translate(struct bpf_prog *prog)
{
	struct nsim_bpf_bound_prog *state = prog->aux->offload->dev_priv;

	state->state = "xlated";
	return 0;
}