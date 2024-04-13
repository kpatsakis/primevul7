rb_reg_new(const char *s, long len, int options)
{
    return rb_enc_reg_new(s, len, rb_ascii8bit_encoding(), options);
}