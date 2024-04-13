hash_findi_add(struct hash_table *hash, const char *str)
{
    struct hash_insert hi;
    void **r;
    char *strx;
    size_t l = strlen(str) + 1;

    r = hash_findib(hash, str, l, &hi);
    if (r)
        return r;

    strx = nasm_malloc(l);  /* Use a more efficient allocator here? */
    memcpy(strx, str, l);
    return hash_add(&hi, strx, NULL);
}