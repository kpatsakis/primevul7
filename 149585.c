match_ary_aref(VALUE match, VALUE idx, VALUE result)
{
    long beg, len;
    int num_regs = RMATCH_REGS(match)->num_regs;

    /* check if idx is Range */
    switch (rb_range_beg_len(idx, &beg, &len, (long)num_regs, !NIL_P(result))) {
      case Qfalse:
	if (NIL_P(result)) return rb_reg_nth_match(NUM2INT(idx), match);
	rb_ary_push(result, rb_reg_nth_match(NUM2INT(idx), match));
	return result;
      case Qnil:
	return Qnil;
      default:
	return match_ary_subseq(match, beg, len, result);
    }
}