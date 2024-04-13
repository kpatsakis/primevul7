match_array(VALUE match, int start)
{
    struct re_registers *regs;
    VALUE ary;
    VALUE target;
    int i;
    int taint = OBJ_TAINTED(match);

    match_check(match);
    regs = RMATCH_REGS(match);
    ary = rb_ary_new2(regs->num_regs);
    target = RMATCH(match)->str;

    for (i=start; i<regs->num_regs; i++) {
	if (regs->beg[i] == -1) {
	    rb_ary_push(ary, Qnil);
	}
	else {
	    VALUE str = rb_str_subseq(target, regs->beg[i], regs->end[i]-regs->beg[i]);
	    if (taint) OBJ_TAINT(str);
	    rb_ary_push(ary, str);
	}
    }
    return ary;
}