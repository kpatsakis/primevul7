oid2addr(oid * id, Ip::Address &addr, u_int size)
{
    struct in_addr i4addr;
    struct in6_addr i6addr;
    u_int i;
    u_char *cp;
    if ( size == sizeof(struct in_addr) )
        cp = (u_char *) &(i4addr.s_addr);
    else
        cp = (u_char *) &(i6addr);
    MemBuf tmp;
    debugs(49, 7, "oid2addr: id : " << snmpDebugOid(id, size, tmp) );
    for (i=0 ; i<size; ++i) {
        cp[i] = id[i];
    }
    if ( size == sizeof(struct in_addr) )
        addr = i4addr;
    else
        addr = i6addr;
}