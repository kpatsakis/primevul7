rb_reg_prepare_re(VALUE re, VALUE str)
{
    onig_errmsg_buffer err = "";
    return rb_reg_prepare_re0(re, str, err);
}