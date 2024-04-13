snmpTreeNext(oid * Current, snint CurrentLen, oid ** Next, snint * NextLen)
{
    oid_ParseFn *Fn = NULL;
    int count = 0;

    debugs(49, 5, "snmpTreeNext: Called");

    MemBuf tmp;
    debugs(49, 6, "snmpTreeNext: Current : " << snmpDebugOid(Current, CurrentLen, tmp));

    mib_tree_entry *mibTreeEntry = mib_tree_head;

    if (mibTreeEntry && Current[count] == mibTreeEntry->name[count]) {
        ++count;

        while ((mibTreeEntry) && (count < CurrentLen) && (!mibTreeEntry->parsefunction)) {
            mib_tree_entry *nextmibTreeEntry = snmpTreeEntry(Current[count], count, mibTreeEntry);

            if (!nextmibTreeEntry)
                break;
            else
                mibTreeEntry = nextmibTreeEntry;

            ++count;
        }
        debugs(49, 5, "snmpTreeNext: Recursed down to requested object");
    } else {
        return NULL;
    }

    if (mibTreeEntry == mib_tree_last)
        return (Fn);

    if ((mibTreeEntry) && (mibTreeEntry->parsefunction)) {
        *NextLen = CurrentLen;
        *Next = (*mibTreeEntry->instancefunction) (Current, NextLen, mibTreeEntry, &Fn);
        if (*Next) {
            debugs(49, 6, "snmpTreeNext: Next : " << snmpDebugOid(*Next, *NextLen, tmp));
            return (Fn);
        }
    }

    if ((mibTreeEntry) && (mibTreeEntry->parsefunction)) {
        --count;
        mib_tree_entry *nextoid = snmpTreeSiblingEntry(Current[count], count, mibTreeEntry->parent);
        if (nextoid) {
            debugs(49, 5, "snmpTreeNext: Next OID found for sibling" << nextoid );
            mibTreeEntry = nextoid;
            ++count;
        } else {
            debugs(49, 5, "snmpTreeNext: Attempting to recurse up for next object");

            while (!nextoid) {
                --count;

                if (mibTreeEntry->parent->parent) {
                    nextoid = mibTreeEntry->parent;
                    mibTreeEntry = snmpTreeEntry(Current[count] + 1, count, nextoid->parent);

                    if (!mibTreeEntry) {
                        mibTreeEntry = nextoid;
                        nextoid = NULL;
                    }
                } else {
                    nextoid = mibTreeEntry;
                    mibTreeEntry = NULL;
                }
            }
        }
    }
    while ((mibTreeEntry) && (!mibTreeEntry->parsefunction)) {
        mibTreeEntry = mibTreeEntry->leaves[0];
    }

    if (mibTreeEntry) {
        *NextLen = mibTreeEntry->len;
        *Next = (*mibTreeEntry->instancefunction) (mibTreeEntry->name, NextLen, mibTreeEntry, &Fn);
    }

    if (*Next) {
        debugs(49, 6, "snmpTreeNext: Next : " << snmpDebugOid(*Next, *NextLen, tmp));
        return (Fn);
    } else
        return NULL;
}