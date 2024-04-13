sk_msg_func_proto(enum bpf_func_id func_id, const struct bpf_prog *prog)
{
	switch (func_id) {
	case BPF_FUNC_msg_redirect_map:
		return &bpf_msg_redirect_map_proto;
	case BPF_FUNC_msg_apply_bytes:
		return &bpf_msg_apply_bytes_proto;
	case BPF_FUNC_msg_cork_bytes:
		return &bpf_msg_cork_bytes_proto;
	case BPF_FUNC_msg_pull_data:
		return &bpf_msg_pull_data_proto;
	default:
		return bpf_base_func_proto(func_id);
	}
}