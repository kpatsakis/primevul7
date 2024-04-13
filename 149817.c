match_aref(int argc, VALUE *argv, VALUE match)
{
    VALUE idx, length;

    match_check(match);
    rb_scan_args(argc, argv, "11", &idx, &length);

    if (NIL_P(length)) {
	if (FIXNUM_P(idx)) {
	    return rb_reg_nth_match(FIX2INT(idx), match);
	}
	else {
	    int num = namev_to_backref_number(RMATCH_REGS(match), RMATCH(match)->regexp, idx);
	    if (num >= 0) {
		return rb_reg_nth_match(num, match);
	    }
	    else {
		return match_ary_aref(match, idx, Qnil);
	    }
	}
    }
    else {
	long beg = NUM2LONG(idx);
	long len = NUM2LONG(length);
	long num_regs = RMATCH_REGS(match)->num_regs;
	if (len < 0) {
	    return Qnil;
	}
	if (beg < 0) {
	    beg += num_regs;
	    if (beg < 0) return Qnil;
	}
	else if (beg > num_regs) {
	    return Qnil;
	}
	else if (beg+len > num_regs) {
	    len = num_regs - beg;
	}
	return match_ary_subseq(match, beg, len, Qnil);
    }
}