snmpAggrType(oid* Current, snint CurrentLen)
{
    debugs(49, 5, HERE);

    mib_tree_entry* mibTreeEntry = mib_tree_head;
    AggrType type = atNone;
    int count = 0;

    if (Current[count] == mibTreeEntry->name[count]) {
        ++count;

        while (mibTreeEntry != NULL && count < CurrentLen) {
            mibTreeEntry = snmpTreeEntry(Current[count], count, mibTreeEntry);
            if (mibTreeEntry != NULL)
                type = mibTreeEntry->aggrType;
            ++count;
        }
    }

    return type;
}