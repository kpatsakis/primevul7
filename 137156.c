void bpf_prog_array_delete_safe(struct bpf_prog_array __rcu *progs,
				struct bpf_prog *old_prog)
{
	struct bpf_prog **prog = progs->progs;

	for (; *prog; prog++)
		if (*prog == old_prog) {
			WRITE_ONCE(*prog, &dummy_bpf_prog.prog);
			break;
		}
}