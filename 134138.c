static Token *make_tok_qstr(Token *next, const char *str)
{
    return make_tok_qstr_len(next, str, strlen(str));
}