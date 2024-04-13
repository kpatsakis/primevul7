match_names(VALUE match)
{
    match_check(match);
    return rb_reg_names(RMATCH(match)->regexp);
}