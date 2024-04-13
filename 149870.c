rb_reg_match_post(VALUE match)
{
    VALUE str;
    long pos;
    struct re_registers *regs;

    if (NIL_P(match)) return Qnil;
    match_check(match);
    regs = RMATCH_REGS(match);
    if (BEG(0) == -1) return Qnil;
    str = RMATCH(match)->str;
    pos = END(0);
    str = rb_str_subseq(str, pos, RSTRING_LEN(str) - pos);
    if (OBJ_TAINTED(match)) OBJ_TAINT(str);
    return str;
}