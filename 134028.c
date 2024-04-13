static Token *make_tok_num(Token *next, int64_t val)
{
    char numbuf[32];
    int len = snprintf(numbuf, sizeof(numbuf), "%"PRId64"", val);
    return new_Token(next, TOK_NUMBER, numbuf, len);
}