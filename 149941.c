match_string(VALUE match)
{
    match_check(match);
    return RMATCH(match)->str;	/* str is frozen */
}