tok_match(const struct Token *a, const struct Token *b)
{
    return a->type == b->type && tok_text_match(a, b);
}