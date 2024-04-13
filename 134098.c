static inline Token *skip_white(Token *x)
{
    while (tok_white(x))
        x = x->next;

    return x;
}