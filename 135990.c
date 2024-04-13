static int nsim_bpf_verifier_prep(struct bpf_prog *prog)
{
	struct nsim_dev *nsim_dev =
			bpf_offload_dev_priv(prog->aux->offload->offdev);

	if (!nsim_dev->bpf_bind_accept)
		return -EOPNOTSUPP;

	return nsim_bpf_create_prog(nsim_dev, prog);
}