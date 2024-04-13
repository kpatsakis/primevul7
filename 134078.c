static inline bool pp_concat_match(const Token *t, unsigned int mask)
{
    return t && (PP_CONCAT_MASK(t->type) & mask);
}