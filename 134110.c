static Token *dup_tlistn(const Token *list, size_t cnt, Token ***tailp)
{
    Token *newlist = NULL;
    Token **tailpp = &newlist;
    const Token *t;

    list_for_each(t, list) {
        Token *nt;
        if (!cnt--)
            break;
        *tailpp = nt = dup_Token(NULL, t);
        tailpp = &nt->next;
    }

    if (tailp) {
        **tailp = newlist;
        if (newlist)
            *tailp = tailpp;
    }

    return newlist;
}