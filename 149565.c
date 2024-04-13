match_regexp(VALUE match)
{
    VALUE regexp;
    match_check(match);
    regexp = RMATCH(match)->regexp;
    if (NIL_P(regexp)) {
	VALUE str = rb_reg_nth_match(0, match);
	regexp = rb_reg_regcomp(rb_reg_quote(str));
	RMATCH(match)->regexp = regexp;
    }
    return regexp;
}