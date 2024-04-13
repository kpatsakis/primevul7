static inline bool tok_text_match(const struct Token *a, const struct Token *b)
{
    return a->len == b->len && !memcmp(tok_text(a), tok_text(b), a->len);
}