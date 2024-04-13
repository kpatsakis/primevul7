rb_reg_last_match(VALUE match)
{
    return rb_reg_nth_match(0, match);
}