static Token *delete_Token(Token *t)
{
    Token *next = t->next;
    nasm_free(t);
    return next;
}