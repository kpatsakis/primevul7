nsim_bpf_verify_insn(struct bpf_verifier_env *env, int insn_idx, int prev_insn)
{
	struct nsim_bpf_bound_prog *state;
	int ret = 0;

	state = env->prog->aux->offload->dev_priv;
	if (state->nsim_dev->bpf_bind_verifier_delay && !insn_idx)
		msleep(state->nsim_dev->bpf_bind_verifier_delay);

	if (insn_idx == env->prog->len - 1) {
		pr_vlog(env, "Hello from netdevsim!\n");

		if (!state->nsim_dev->bpf_bind_verifier_accept)
			ret = -EOPNOTSUPP;
	}

	return ret;
}