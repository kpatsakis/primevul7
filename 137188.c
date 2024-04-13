static bool sock_ops_is_valid_access(int off, int size,
				     enum bpf_access_type type,
				     const struct bpf_prog *prog,
				     struct bpf_insn_access_aux *info)
{
	const int size_default = sizeof(__u32);

	if (off < 0 || off >= sizeof(struct bpf_sock_ops))
		return false;

	/* The verifier guarantees that size > 0. */
	if (off % size != 0)
		return false;

	if (type == BPF_WRITE) {
		switch (off) {
		case offsetof(struct bpf_sock_ops, reply):
		case offsetof(struct bpf_sock_ops, sk_txhash):
			if (size != size_default)
				return false;
			break;
		default:
			return false;
		}
	} else {
		switch (off) {
		case bpf_ctx_range_till(struct bpf_sock_ops, bytes_received,
					bytes_acked):
			if (size != sizeof(__u64))
				return false;
			break;
		default:
			if (size != size_default)
				return false;
			break;
		}
	}

	return true;
}