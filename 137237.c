int bpf_prog_array_length(struct bpf_prog_array __rcu *progs)
{
	struct bpf_prog **prog;
	u32 cnt = 0;

	rcu_read_lock();
	prog = rcu_dereference(progs)->progs;
	for (; *prog; prog++)
		if (*prog != &dummy_bpf_prog.prog)
			cnt++;
	rcu_read_unlock();
	return cnt;
}