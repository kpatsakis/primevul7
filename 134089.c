static char *dup_text(const struct Token *t)
{
    size_t size = t->len + 1;
    char *p = nasm_malloc(size);

    return memcpy(p, tok_text(t), size);
}