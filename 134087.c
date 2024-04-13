static inline Token *alloc_Token(void)
{
    Token *t;
    nasm_new(*t);
    return t;
}