static inline bool tok_isnt(const Token *x, char c)
{
    return x && !((x->text.a[0] == c) & !x->text.a[1]);
}