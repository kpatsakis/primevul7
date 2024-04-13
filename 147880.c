time_Inst(oid * name, snint * len, mib_tree_entry * current, oid_ParseFn ** Fn)
{
    oid *instance = NULL;
    int identifier = 0, loop = 0;
    int index[TIME_INDEX_LEN] = {TIME_INDEX};

    if (*len <= current->len) {
        instance = (oid *)xmalloc(sizeof(*name) * (*len + 1));
        memcpy(instance, name, sizeof(*name) * (*len));
        instance[*len] = *index;
        *len += 1;
    } else {
        identifier = name[*len - 1];

        while ((loop < TIME_INDEX_LEN) && (identifier != index[loop]))
            ++loop;

        if (loop < (TIME_INDEX_LEN - 1)) {
            instance = (oid *)xmalloc(sizeof(*name) * (*len));
            memcpy(instance, name, sizeof(*name) * (*len));
            instance[*len - 1] = index[++loop];
        }
    }

    *Fn = current->parsefunction;
    return (instance);
}