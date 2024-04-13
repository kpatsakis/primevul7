rb_reg_new_ary(VALUE ary, int opt)
{
    return rb_reg_new_str(rb_reg_preprocess_dregexp(ary, opt), opt);
}