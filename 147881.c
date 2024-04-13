snmpTreeSiblingEntry(oid entry, snint len, mib_tree_entry * current)
{
    mib_tree_entry *next = NULL;
    int count = 0;

    while ((!next) && (count < current->children)) {
        if (current->leaves[count]->name[len] == entry) {
            next = current->leaves[count];
        }

        ++count;
    }

    /* Exactly the sibling on right */
    if (count < current->children) {
        next = current->leaves[count];
    } else {
        next = NULL;
    }

    return (next);
}