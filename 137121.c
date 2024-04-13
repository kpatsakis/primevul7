static bool __sock_filter_check_attach_type(int off,
					    enum bpf_access_type access_type,
					    enum bpf_attach_type attach_type)
{
	switch (off) {
	case offsetof(struct bpf_sock, bound_dev_if):
	case offsetof(struct bpf_sock, mark):
	case offsetof(struct bpf_sock, priority):
		switch (attach_type) {
		case BPF_CGROUP_INET_SOCK_CREATE:
			goto full_access;
		default:
			return false;
		}
	case bpf_ctx_range(struct bpf_sock, src_ip4):
		switch (attach_type) {
		case BPF_CGROUP_INET4_POST_BIND:
			goto read_only;
		default:
			return false;
		}
	case bpf_ctx_range_till(struct bpf_sock, src_ip6[0], src_ip6[3]):
		switch (attach_type) {
		case BPF_CGROUP_INET6_POST_BIND:
			goto read_only;
		default:
			return false;
		}
	case bpf_ctx_range(struct bpf_sock, src_port):
		switch (attach_type) {
		case BPF_CGROUP_INET4_POST_BIND:
		case BPF_CGROUP_INET6_POST_BIND:
			goto read_only;
		default:
			return false;
		}
	}
read_only:
	return access_type == BPF_READ;
full_access:
	return true;
}