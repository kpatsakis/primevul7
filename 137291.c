static bool sock_filter_is_valid_access(int off, int size,
					enum bpf_access_type type,
					const struct bpf_prog *prog,
					struct bpf_insn_access_aux *info)
{
	if (off < 0 || off >= sizeof(struct bpf_sock))
		return false;
	if (off % size != 0)
		return false;
	if (!__sock_filter_check_attach_type(off, type,
					     prog->expected_attach_type))
		return false;
	if (!__sock_filter_check_size(off, size, info))
		return false;
	return true;
}