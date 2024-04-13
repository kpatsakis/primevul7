snmpLookupNodeStr(mib_tree_entry *root, const char *str)
{
    oid *name;
    int namelen;
    mib_tree_entry *e;

    if (root)
        e = root;
    else
        e = mib_tree_head;

    if (! snmpCreateOidFromStr(str, &name, &namelen))
        return NULL;

    /* I wish there were some kind of sensible existing tree traversal
     * routine to use. I'll worry about that later */
    if (namelen <= 1) {
        xfree(name);
        return e;       /* XXX it should only be this? */
    }

    int i, r = 1;
    while (r < namelen) {

        /* Find the child node which matches this */
        for (i = 0; i < e->children && e->leaves[i]->name[r] != name[r]; ++i) ; // seek-loop

        /* Are we pointing to that node? */
        if (i >= e->children)
            break;
        assert(e->leaves[i]->name[r] == name[r]);

        /* Skip to that node! */
        e = e->leaves[i];
        ++r;
    }

    xfree(name);
    return e;
}