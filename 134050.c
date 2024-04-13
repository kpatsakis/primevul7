static inline bool tok_white(const Token *x)
{
    return tok_type(x, TOK_WHITESPACE);
}