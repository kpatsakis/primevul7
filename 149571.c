rb_reg_nth_defined(int nth, VALUE match)
{
    struct re_registers *regs;
    if (NIL_P(match)) return Qnil;
    match_check(match);
    regs = RMATCH_REGS(match);
    if (nth >= regs->num_regs) {
	return Qnil;
    }
    if (nth < 0) {
	nth += regs->num_regs;
	if (nth <= 0) return Qnil;
    }
    if (BEG(nth) == -1) return Qfalse;
    return Qtrue;
}