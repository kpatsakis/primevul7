snmpAddNodeChild(mib_tree_entry *entry, mib_tree_entry *child)
{
    debugs(49, 5, "snmpAddNodeChild: assigning " << child << " to parent " << entry);
    entry->leaves = (mib_tree_entry **)xrealloc(entry->leaves, sizeof(mib_tree_entry *) * (entry->children + 1));
    entry->leaves[entry->children] = child;
    entry->leaves[entry->children]->parent = entry;
    ++ entry->children;
}