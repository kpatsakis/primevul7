void bpf_prog_kallsyms_del(struct bpf_prog *fp)
{
	if (!bpf_prog_kallsyms_candidate(fp))
		return;

	spin_lock_bh(&bpf_lock);
	bpf_prog_ksym_node_del(fp->aux);
	spin_unlock_bh(&bpf_lock);
}