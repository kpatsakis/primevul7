postmatch_getter(void)
{
    return rb_reg_match_post(rb_backref_get());
}