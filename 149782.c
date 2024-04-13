rb_reg_match2(VALUE re)
{
    long start;
    VALUE line = rb_lastline_get();

    if (!RB_TYPE_P(line, T_STRING)) {
	rb_backref_set(Qnil);
	return Qnil;
    }

    start = rb_reg_search(re, line, 0, 0);
    if (start < 0) {
	return Qnil;
    }
    start = rb_str_sublen(line, start);
    return LONG2FIX(start);
}