static Token *make_tok_char(Token *next, char op)
{
    Token *t = new_Token(next, TOK_OTHER, NULL, 1);
    t->text.a[0] = op;
    return t;
}