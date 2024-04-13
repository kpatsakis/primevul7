static Token *dup_tlist_reverse(const Token *list, Token *tail)
{
    const Token *t;

    list_for_each(t, list)
        tail = dup_Token(tail, t);

    return tail;
}