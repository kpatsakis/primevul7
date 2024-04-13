static bool bpf_skb_is_valid_access(int off, int size, enum bpf_access_type type,
				    const struct bpf_prog *prog,
				    struct bpf_insn_access_aux *info)
{
	const int size_default = sizeof(__u32);

	if (off < 0 || off >= sizeof(struct __sk_buff))
		return false;

	/* The verifier guarantees that size > 0. */
	if (off % size != 0)
		return false;

	switch (off) {
	case bpf_ctx_range_till(struct __sk_buff, cb[0], cb[4]):
		if (off + size > offsetofend(struct __sk_buff, cb[4]))
			return false;
		break;
	case bpf_ctx_range_till(struct __sk_buff, remote_ip6[0], remote_ip6[3]):
	case bpf_ctx_range_till(struct __sk_buff, local_ip6[0], local_ip6[3]):
	case bpf_ctx_range_till(struct __sk_buff, remote_ip4, remote_ip4):
	case bpf_ctx_range_till(struct __sk_buff, local_ip4, local_ip4):
	case bpf_ctx_range(struct __sk_buff, data):
	case bpf_ctx_range(struct __sk_buff, data_meta):
	case bpf_ctx_range(struct __sk_buff, data_end):
		if (size != size_default)
			return false;
		break;
	default:
		/* Only narrow read access allowed for now. */
		if (type == BPF_WRITE) {
			if (size != size_default)
				return false;
		} else {
			bpf_ctx_record_field_size(info, size_default);
			if (!bpf_ctx_narrow_access_ok(off, size, size_default))
				return false;
		}
	}

	return true;
}