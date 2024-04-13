rb_reg_eqq(VALUE re, VALUE str)
{
    long start;

    str = reg_operand(str, FALSE);
    if (NIL_P(str)) {
	rb_backref_set(Qnil);
	return Qfalse;
    }
    start = rb_reg_search(re, str, 0, 0);
    if (start < 0) {
	return Qfalse;
    }
    return Qtrue;
}