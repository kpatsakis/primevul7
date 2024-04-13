static void free_mmacro_table(struct hash_table *mmt)
{
    struct hash_iterator it;
    const struct hash_node *np;

    hash_for_each(mmt, it, np) {
        MMacro *tmp;
        MMacro *m = np->data;
        nasm_free((void *)np->key);
        list_for_each_safe(m, tmp, m)
            free_mmacro(m);
    }
    hash_free(mmt);
}