static int bpf_adj_branches(struct bpf_prog *prog, u32 pos, u32 delta,
			    const bool probe_pass)
{
	u32 i, insn_cnt = prog->len + (probe_pass ? delta : 0);
	struct bpf_insn *insn = prog->insnsi;
	int ret = 0;

	for (i = 0; i < insn_cnt; i++, insn++) {
		u8 code;

		/* In the probing pass we still operate on the original,
		 * unpatched image in order to check overflows before we
		 * do any other adjustments. Therefore skip the patchlet.
		 */
		if (probe_pass && i == pos) {
			i += delta + 1;
			insn++;
		}
		code = insn->code;
		if (BPF_CLASS(code) != BPF_JMP ||
		    BPF_OP(code) == BPF_EXIT)
			continue;
		/* Adjust offset of jmps if we cross patch boundaries. */
		if (BPF_OP(code) == BPF_CALL) {
			if (insn->src_reg != BPF_PSEUDO_CALL)
				continue;
			ret = bpf_adj_delta_to_imm(insn, pos, delta, i,
						   probe_pass);
		} else {
			ret = bpf_adj_delta_to_off(insn, pos, delta, i,
						   probe_pass);
		}
		if (ret)
			break;
	}

	return ret;
}