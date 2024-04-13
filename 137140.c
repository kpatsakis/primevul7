static bool __sock_filter_check_size(int off, int size,
				     struct bpf_insn_access_aux *info)
{
	const int size_default = sizeof(__u32);

	switch (off) {
	case bpf_ctx_range(struct bpf_sock, src_ip4):
	case bpf_ctx_range_till(struct bpf_sock, src_ip6[0], src_ip6[3]):
		bpf_ctx_record_field_size(info, size_default);
		return bpf_ctx_narrow_access_ok(off, size, size_default);
	}

	return size == size_default;
}