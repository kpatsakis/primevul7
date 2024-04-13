rb_reg_compile(VALUE str, int options, const char *sourcefile, int sourceline)
{
    VALUE re = rb_reg_alloc();
    onig_errmsg_buffer err = "";

    if (!str) str = rb_str_new(0,0);
    if (rb_reg_initialize_str(re, str, options, err, sourcefile, sourceline) != 0) {
	rb_set_errinfo(rb_reg_error_desc(str, options, err));
	return Qnil;
    }
    FL_SET(re, REG_LITERAL);
    return re;
}