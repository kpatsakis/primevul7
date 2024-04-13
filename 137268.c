static bool sock_addr_is_valid_access(int off, int size,
				      enum bpf_access_type type,
				      const struct bpf_prog *prog,
				      struct bpf_insn_access_aux *info)
{
	const int size_default = sizeof(__u32);

	if (off < 0 || off >= sizeof(struct bpf_sock_addr))
		return false;
	if (off % size != 0)
		return false;

	/* Disallow access to IPv6 fields from IPv4 contex and vise
	 * versa.
	 */
	switch (off) {
	case bpf_ctx_range(struct bpf_sock_addr, user_ip4):
		switch (prog->expected_attach_type) {
		case BPF_CGROUP_INET4_BIND:
		case BPF_CGROUP_INET4_CONNECT:
			break;
		default:
			return false;
		}
		break;
	case bpf_ctx_range_till(struct bpf_sock_addr, user_ip6[0], user_ip6[3]):
		switch (prog->expected_attach_type) {
		case BPF_CGROUP_INET6_BIND:
		case BPF_CGROUP_INET6_CONNECT:
			break;
		default:
			return false;
		}
		break;
	}

	switch (off) {
	case bpf_ctx_range(struct bpf_sock_addr, user_ip4):
	case bpf_ctx_range_till(struct bpf_sock_addr, user_ip6[0], user_ip6[3]):
		/* Only narrow read access allowed for now. */
		if (type == BPF_READ) {
			bpf_ctx_record_field_size(info, size_default);
			if (!bpf_ctx_narrow_access_ok(off, size, size_default))
				return false;
		} else {
			if (size != size_default)
				return false;
		}
		break;
	case bpf_ctx_range(struct bpf_sock_addr, user_port):
		if (size != size_default)
			return false;
		break;
	default:
		if (type == BPF_READ) {
			if (size != size_default)
				return false;
		} else {
			return false;
		}
	}

	return true;
}