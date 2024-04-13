hash_findi_add(struct hash_table *hash, const char *str)
{
    struct hash_insert hi;
    void **r;
    char *strx;

    r = hash_findi(hash, str, &hi);
    if (r)
        return r;

    strx = nasm_strdup(str);    /* Use a more efficient allocator here? */
    return hash_add(&hi, strx, NULL);
}