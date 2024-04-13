struct bpf_prog_array __rcu *bpf_prog_array_alloc(u32 prog_cnt, gfp_t flags)
{
	if (prog_cnt)
		return kzalloc(sizeof(struct bpf_prog_array) +
			       sizeof(struct bpf_prog *) * (prog_cnt + 1),
			       flags);

	return &empty_prog_array.hdr;
}