snmpTreeGet(oid * Current, snint CurrentLen)
{
    oid_ParseFn *Fn = NULL;
    mib_tree_entry *mibTreeEntry = NULL;
    int count = 0;

    debugs(49, 5, "snmpTreeGet: Called");

    MemBuf tmp;
    debugs(49, 6, "snmpTreeGet: Current : " << snmpDebugOid(Current, CurrentLen, tmp) );

    mibTreeEntry = mib_tree_head;

    if (Current[count] == mibTreeEntry->name[count]) {
        ++count;

        while ((mibTreeEntry) && (count < CurrentLen) && (!mibTreeEntry->parsefunction)) {
            mibTreeEntry = snmpTreeEntry(Current[count], count, mibTreeEntry);
            ++count;
        }
    }

    if (mibTreeEntry && mibTreeEntry->parsefunction)
        Fn = mibTreeEntry->parsefunction;

    debugs(49, 5, "snmpTreeGet: return");

    return (Fn);
}