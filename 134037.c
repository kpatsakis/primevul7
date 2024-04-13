static void clear_smacro_table(struct hash_table *smt, enum clear_what what)
{
    struct hash_iterator it;
    const struct hash_node *np;
    bool empty = true;

    /*
     * Walk the hash table and clear out anything we don't want
     */
    hash_for_each(smt, it, np) {
        SMacro *tmp;
        SMacro *s = np->data;
        SMacro **head = (SMacro **)&np->data;

        list_for_each_safe(s, tmp, s) {
            if (what & ((enum clear_what)s->alias + 1)) {
                *head = s->next;
                free_smacro(s);
            } else {
                empty = false;
            }
        }
    }

    /*
     * Free the hash table and keys if and only if it is now empty.
     * Note: we cannot free keys even for an empty list above, as that
     * mucks up the hash algorithm.
     */
    if (empty)
        hash_free_all(smt, true);
}