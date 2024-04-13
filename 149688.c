rb_reg_match(VALUE re, VALUE str)
{
    long pos = reg_match_pos(re, &str, 0);
    if (pos < 0) return Qnil;
    pos = rb_str_sublen(str, pos);
    return LONG2FIX(pos);
}