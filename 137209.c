void bpf_prog_array_free(struct bpf_prog_array __rcu *progs)
{
	if (!progs ||
	    progs == (struct bpf_prog_array __rcu *)&empty_prog_array.hdr)
		return;
	kfree_rcu(progs, rcu);
}