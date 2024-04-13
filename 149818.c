reg_set_source(VALUE reg, VALUE str, rb_encoding *enc)
{
    rb_encoding *regenc = rb_enc_get(reg);
    if (regenc != enc) {
	str = rb_enc_associate(rb_str_dup(str), enc = regenc);
    }
    RB_OBJ_WRITE(reg, &RREGEXP(reg)->src, rb_fstring(str));
}