void bpf_prog_kallsyms_add(struct bpf_prog *fp)
{
	if (!bpf_prog_kallsyms_candidate(fp) ||
	    !capable(CAP_SYS_ADMIN))
		return;

	spin_lock_bh(&bpf_lock);
	bpf_prog_ksym_node_add(fp->aux);
	spin_unlock_bh(&bpf_lock);
}