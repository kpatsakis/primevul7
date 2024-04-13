Token *quote_token(Token *t)
{
    if (likely(!tok_is(t, TOK_INTERNAL_STRING)))
	return t;

    return quote_any_token(t);
}