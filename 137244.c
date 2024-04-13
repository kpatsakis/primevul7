static int bpf_adj_delta_to_off(struct bpf_insn *insn, u32 pos, u32 delta,
				u32 curr, const bool probe_pass)
{
	const s32 off_min = S16_MIN, off_max = S16_MAX;
	s32 off = insn->off;

	if (curr < pos && curr + off + 1 > pos)
		off += delta;
	else if (curr > pos + delta && curr + off + 1 <= pos + delta)
		off -= delta;
	if (off < off_min || off > off_max)
		return -ERANGE;
	if (!probe_pass)
		insn->off = off;
	return 0;
}