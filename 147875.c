peer_Inst(oid * name, snint * len, mib_tree_entry * current, oid_ParseFn ** Fn)
{
    oid *instance = NULL;
    CachePeer *peers = Config.peers;

    if (peers == NULL) {
        debugs(49, 6, "snmp peer_Inst: No Peers.");
        current = current->parent->parent->parent->leaves[1];
        while ((current) && (!current->parsefunction))
            current = current->leaves[0];

        if (!current)
            return (instance);

        instance = client_Inst(current->name, len, current, Fn);
    } else if (*len <= current->len) {
        debugs(49, 6, "snmp peer_Inst: *len <= current->len ???");
        instance = (oid *)xmalloc(sizeof(*name) * ( *len + 1));
        memcpy(instance, name, sizeof(*name) * (*len));
        instance[*len] = 1 ;
        *len += 1;
    } else {
        int no = name[current->len] ;
        int i;
        // Note: This works because the Config.peers keeps its index according to its position.
        for ( i=0 ; peers && (i < no) ; peers = peers->next , ++i ) ;

        if (peers) {
            debugs(49, 6, "snmp peer_Inst: Encode peer #" << i);
            instance = (oid *)xmalloc(sizeof(*name) * (current->len + 1 ));
            memcpy(instance, name, (sizeof(*name) * current->len ));
            instance[current->len] = no + 1 ; // i.e. the next index on cache_peeer table.
        } else {
            debugs(49, 6, "snmp peer_Inst: We have " << i << " peers. Can't find #" << no);
            return (instance);
        }
    }
    *Fn = current->parsefunction;
    return (instance);
}