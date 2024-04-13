rb_enc_reg_error_desc(const char *s, long len, rb_encoding *enc, int options, const char *err)
{
    char opts[6];
    VALUE desc = rb_str_buf_new2(err);
    rb_encoding *resenc = rb_default_internal_encoding();
    if (resenc == NULL) resenc = rb_default_external_encoding();

    rb_enc_associate(desc, enc);
    rb_str_buf_cat2(desc, ": /");
    rb_reg_expr_str(desc, s, len, enc, resenc);
    opts[0] = '/';
    option_to_str(opts + 1, options);
    rb_str_buf_cat2(desc, opts);
    return rb_exc_new3(rb_eRegexpError, desc);
}