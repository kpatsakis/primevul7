static bool xdp_is_valid_access(int off, int size,
				enum bpf_access_type type,
				const struct bpf_prog *prog,
				struct bpf_insn_access_aux *info)
{
	if (type == BPF_WRITE)
		return false;

	switch (off) {
	case offsetof(struct xdp_md, data):
		info->reg_type = PTR_TO_PACKET;
		break;
	case offsetof(struct xdp_md, data_meta):
		info->reg_type = PTR_TO_PACKET_META;
		break;
	case offsetof(struct xdp_md, data_end):
		info->reg_type = PTR_TO_PACKET_END;
		break;
	}

	return __is_valid_xdp_access(off, size);
}