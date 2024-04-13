rb_enc_reg_new(const char *s, long len, rb_encoding *enc, int options)
{
    VALUE re = rb_reg_alloc();
    onig_errmsg_buffer err = "";

    if (rb_reg_initialize(re, s, len, enc, options, err, NULL, 0) != 0) {
	rb_enc_reg_raise(s, len, enc, options, err);
    }
    RB_OBJ_WRITE(re, &RREGEXP(re)->src, rb_fstring(rb_enc_str_new(s, len, enc)));

    return re;
}