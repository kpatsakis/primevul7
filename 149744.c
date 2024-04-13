rb_reg_desc(const char *s, long len, VALUE re)
{
    rb_encoding *enc = rb_enc_get(re);
    VALUE str = rb_str_buf_new2("/");
    rb_encoding *resenc = rb_default_internal_encoding();
    if (resenc == NULL) resenc = rb_default_external_encoding();

    if (re && rb_enc_asciicompat(enc)) {
	rb_enc_copy(str, re);
    }
    else {
	rb_enc_associate(str, rb_usascii_encoding());
    }
    rb_reg_expr_str(str, s, len, enc, resenc);
    rb_str_buf_cat2(str, "/");
    if (re) {
	char opts[4];
	rb_reg_check(re);
	if (*option_to_str(opts, RREGEXP_PTR(re)->options))
	    rb_str_buf_cat2(str, opts);
	if (RBASIC(re)->flags & REG_ENCODING_NONE)
	    rb_str_buf_cat2(str, "n");
    }
    OBJ_INFECT(str, re);
    return str;
}