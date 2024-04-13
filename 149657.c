rb_reg_nth_match(int nth, VALUE match)
{
    VALUE str;
    long start, end, len;
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
    start = BEG(nth);
    if (start == -1) return Qnil;
    end = END(nth);
    len = end - start;
    str = rb_str_subseq(RMATCH(match)->str, start, len);
    OBJ_INFECT(str, match);
    return str;
}