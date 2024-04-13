snmpDebugOid(oid * Name, snint Len, MemBuf &outbuf)
{
    char mbuf[16];
    int x;
    if (outbuf.isNull())
        outbuf.init(16, MAX_IPSTRLEN);

    for (x = 0; x < Len; ++x) {
        size_t bytes = snprintf(mbuf, sizeof(mbuf), ".%u", (unsigned int) Name[x]);
        outbuf.append(mbuf, bytes);
    }
    return outbuf.content();
}