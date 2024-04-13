static int bpf_adj_delta_to_imm(struct bpf_insn *insn, u32 pos, u32 delta,
				u32 curr, const bool probe_pass)
{
	const s64 imm_min = S32_MIN, imm_max = S32_MAX;
	s64 imm = insn->imm;

	if (curr < pos && curr + imm + 1 > pos)
		imm += delta;
	else if (curr > pos + delta && curr + imm + 1 <= pos + delta)
		imm -= delta;
	if (imm < imm_min || imm > imm_max)
		return -ERANGE;
	if (!probe_pass)
		insn->imm = imm;
	return 0;
}