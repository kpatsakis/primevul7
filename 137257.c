static u32 sk_msg_convert_ctx_access(enum bpf_access_type type,
				     const struct bpf_insn *si,
				     struct bpf_insn *insn_buf,
				     struct bpf_prog *prog, u32 *target_size)
{
	struct bpf_insn *insn = insn_buf;

	switch (si->off) {
	case offsetof(struct sk_msg_md, data):
		*insn++ = BPF_LDX_MEM(BPF_FIELD_SIZEOF(struct sk_msg_buff, data),
				      si->dst_reg, si->src_reg,
				      offsetof(struct sk_msg_buff, data));
		break;
	case offsetof(struct sk_msg_md, data_end):
		*insn++ = BPF_LDX_MEM(BPF_FIELD_SIZEOF(struct sk_msg_buff, data_end),
				      si->dst_reg, si->src_reg,
				      offsetof(struct sk_msg_buff, data_end));
		break;
	}

	return insn - insn_buf;
}