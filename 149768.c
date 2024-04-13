rb_match_count(VALUE match)
{
    struct re_registers *regs;
    if (NIL_P(match)) return -1;
    regs = RMATCH_REGS(match);
    if (!regs) return -1;
    return regs->num_regs;
}