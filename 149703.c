match_to_s(VALUE match)
{
    VALUE str = rb_reg_last_match(match);

    match_check(match);
    if (NIL_P(str)) str = rb_str_new(0,0);
    if (OBJ_TAINTED(match)) OBJ_TAINT(str);
    if (OBJ_TAINTED(RMATCH(match)->str)) OBJ_TAINT(str);
    return str;
}