match_alloc(VALUE klass)
{
    NEWOBJ_OF(match, struct RMatch, klass, T_MATCH);

    match->str = 0;
    match->rmatch = 0;
    match->regexp = 0;
    match->rmatch = ZALLOC(struct rmatch);

    return (VALUE)match;
}