snmpAddNode(oid * name, int len, oid_ParseFn * parsefunction, instance_Fn * instancefunction, AggrType aggrType, int children,...)
{
    va_list args;
    int loop;
    mib_tree_entry *entry = NULL;
    va_start(args, children);

    MemBuf tmp;
    debugs(49, 6, "snmpAddNode: Children : " << children << ", Oid : " << snmpDebugOid(name, len, tmp));

    va_start(args, children);
    entry = (mib_tree_entry *)xmalloc(sizeof(mib_tree_entry));
    entry->name = name;
    entry->len = len;
    entry->parsefunction = parsefunction;
    entry->instancefunction = instancefunction;
    entry->children = children;
    entry->leaves = NULL;
    entry->aggrType = aggrType;

    if (children > 0) {
        entry->leaves = (mib_tree_entry **)xmalloc(sizeof(mib_tree_entry *) * children);

        for (loop = 0; loop < children; ++loop) {
            entry->leaves[loop] = va_arg(args, mib_tree_entry *);
            entry->leaves[loop]->parent = entry;
        }
    }

    va_end(args);
    return (entry);
}