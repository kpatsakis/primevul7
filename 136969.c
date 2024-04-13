_XkbCheckRequestBounds(ClientPtr client, void *stuff, void *from, void *to) {
    char *cstuff = (char *)stuff;
    char *cfrom = (char *)from;
    char *cto = (char *)to;

    return cfrom < cto &&
           cfrom >= cstuff &&
           cfrom < cstuff + ((size_t)client->req_len << 2) &&
           cto >= cstuff &&
           cto <= cstuff + ((size_t)client->req_len << 2);
}