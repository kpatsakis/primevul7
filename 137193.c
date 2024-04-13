static bool sk_msg_is_valid_access(int off, int size,
				   enum bpf_access_type type,
				   const struct bpf_prog *prog,
				   struct bpf_insn_access_aux *info)
{
	if (type == BPF_WRITE)
		return false;

	switch (off) {
	case offsetof(struct sk_msg_md, data):
		info->reg_type = PTR_TO_PACKET;
		break;
	case offsetof(struct sk_msg_md, data_end):
		info->reg_type = PTR_TO_PACKET_END;
		break;
	}

	if (off < 0 || off >= sizeof(struct sk_msg_md))
		return false;
	if (off % size != 0)
		return false;
	if (size != sizeof(__u64))
		return false;

	return true;
}