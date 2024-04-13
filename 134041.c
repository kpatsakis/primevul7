static Token *delete_Token(Token *t)
{
    Token *next = t->next;

    nasm_zero(*t);
    t->next = freeTokens;
    freeTokens = t;

    return next;
}