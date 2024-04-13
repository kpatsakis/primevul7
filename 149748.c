rb_reg_check(VALUE re)
{
    if (!RREGEXP_PTR(re) || !RREGEXP_SRC(re) || !RREGEXP_SRC_PTR(re)) {
	rb_raise(rb_eTypeError, "uninitialized Regexp");
    }
}