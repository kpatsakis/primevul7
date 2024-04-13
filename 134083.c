static Token *make_tok_qstr_len(Token *next, const char *str, size_t len)
{
    char *p = nasm_quote(str, &len);
    return new_Token_free(next, TOK_STRING, p, len);
}