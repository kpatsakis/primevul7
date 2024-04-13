snmpAddNodeStr(const char *base_str, int o, oid_ParseFn * parsefunction, instance_Fn * instancefunction, AggrType aggrType)
{
    mib_tree_entry *m, *b;
    oid *n;
    int nl;
    char s[1024];

    /* Find base node */
    b = snmpLookupNodeStr(mib_tree_head, base_str);
    if (! b)
        return NULL;
    debugs(49, 5, "snmpAddNodeStr: " << base_str << ": -> " << b);

    /* Create OID string for new entry */
    snprintf(s, 1024, "%s.%d", base_str, o);
    if (! snmpCreateOidFromStr(s, &n, &nl))
        return NULL;

    /* Create a node */
    m = snmpAddNode(n, nl, parsefunction, instancefunction, aggrType, 0);

    /* Link it into the existing tree */
    snmpAddNodeChild(b, m);

    /* Return the node */
    return m;
}