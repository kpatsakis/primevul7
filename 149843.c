match_size(VALUE match)
{
    match_check(match);
    return INT2FIX(RMATCH_REGS(match)->num_regs);
}