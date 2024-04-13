rb_match_nth_defined(int nth, VALUE match)
{
    struct re_registers *regs;
    if (NIL_P(match)) return FALSE;
    regs = RMATCH_REGS(match);
    if (!regs) return FALSE;
    if (nth >= regs->num_regs) {
	return FALSE;
    }
    if (nth < 0) {
	nth += regs->num_regs;
	if (nth <= 0) return FALSE;
    }
    return (BEG(nth) != -1);
}