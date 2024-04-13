static bool lwt_is_valid_access(int off, int size,
				enum bpf_access_type type,
				const struct bpf_prog *prog,
				struct bpf_insn_access_aux *info)
{
	switch (off) {
	case bpf_ctx_range(struct __sk_buff, tc_classid):
	case bpf_ctx_range_till(struct __sk_buff, family, local_port):
	case bpf_ctx_range(struct __sk_buff, data_meta):
		return false;
	}

	if (type == BPF_WRITE) {
		switch (off) {
		case bpf_ctx_range(struct __sk_buff, mark):
		case bpf_ctx_range(struct __sk_buff, priority):
		case bpf_ctx_range_till(struct __sk_buff, cb[0], cb[4]):
			break;
		default:
			return false;
		}
	}

	switch (off) {
	case bpf_ctx_range(struct __sk_buff, data):
		info->reg_type = PTR_TO_PACKET;
		break;
	case bpf_ctx_range(struct __sk_buff, data_end):
		info->reg_type = PTR_TO_PACKET_END;
		break;
	}

	return bpf_skb_is_valid_access(off, size, type, prog, info);
}