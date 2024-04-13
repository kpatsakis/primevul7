static int tc_cls_act_prologue(struct bpf_insn *insn_buf, bool direct_write,
			       const struct bpf_prog *prog)
{
	return bpf_unclone_prologue(insn_buf, direct_write, prog, TC_ACT_SHOT);
}