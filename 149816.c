rb_reg_raise(const char *s, long len, const char *err, VALUE re)
{
    VALUE desc = rb_reg_desc(s, len, re);

    rb_raise(rb_eRegexpError, "%s: %"PRIsVALUE, err, desc);
}