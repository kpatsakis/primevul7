static Token *new_White(Token *next)
{
    Token *t = alloc_Token();

    t->next = next;
    t->type = TOK_WHITESPACE;
    t->len  = 1;
    t->text.a[0] = ' ';

    return t;
}