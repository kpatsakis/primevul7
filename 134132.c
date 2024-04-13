static Token *dup_tlist(const Token *list, Token ***tailp)
{
    Token *newlist = NULL;
    Token **tailpp = &newlist;
    const Token *t;

    list_for_each(t, list) {
        Token *nt;
        *tailpp = nt = dup_Token(NULL, t);
        tailpp = &nt->next;
    }

    if (tailp) {
        **tailp = newlist;
        *tailp = tailpp;
    }

    return newlist;
}