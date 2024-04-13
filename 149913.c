match_set_string(VALUE m, VALUE string, long pos, long len)
{
    struct RMatch *match = (struct RMatch *)m;
    struct rmatch *rmatch = match->rmatch;

    match->str = string;
    match->regexp = Qnil;
    onig_region_resize(&rmatch->regs, 1);
    rmatch->regs.beg[0] = pos;
    rmatch->regs.end[0] = pos + len;
    rmatch->char_offset_updated = 0;
    OBJ_INFECT(match, string);
}