match_getter(void)
{
    VALUE match = rb_backref_get();

    if (NIL_P(match)) return Qnil;
    rb_match_busy(match);
    return match;
}