static_Inst(oid * name, snint * len, mib_tree_entry * current, oid_ParseFn ** Fn)
{
    oid *instance = NULL;
    if (*len <= current->len) {
        instance = (oid *)xmalloc(sizeof(*name) * (*len + 1));
        memcpy(instance, name, sizeof(*name) * (*len));
        instance[*len] = 0;
        *len += 1;
    }
    *Fn = current->parsefunction;
    return (instance);
}