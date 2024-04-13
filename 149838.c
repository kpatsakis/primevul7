match_ary_subseq(VALUE match, long beg, long len, VALUE result)
{
    long olen = RMATCH_REGS(match)->num_regs;
    long j, end = olen < beg+len ? olen : beg+len;
    if (NIL_P(result)) result = rb_ary_new_capa(len);
    if (len == 0) return result;

    for (j = beg; j < end; j++) {
	rb_ary_push(result, rb_reg_nth_match((int)j, match));
    }
    if (beg + len > j) {
	rb_ary_resize(result, RARRAY_LEN(result) + (beg + len) - j);
    }
    return result;
}