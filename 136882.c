ProcXkbUseExtension(ClientPtr client)
{
    REQUEST(xkbUseExtensionReq);
    xkbUseExtensionReply rep;
    int supported;

    REQUEST_SIZE_MATCH(xkbUseExtensionReq);
    if (stuff->wantedMajor != SERVER_XKB_MAJOR_VERSION) {
        /* pre-release version 0.65 is compatible with 1.00 */
        supported = ((SERVER_XKB_MAJOR_VERSION == 1) &&
                     (stuff->wantedMajor == 0) && (stuff->wantedMinor == 65));
    }
    else
        supported = 1;

    if ((supported) && (!(client->xkbClientFlags & _XkbClientInitialized))) {
        client->xkbClientFlags = _XkbClientInitialized;
        if (stuff->wantedMajor == 0)
            client->xkbClientFlags |= _XkbClientIsAncient;
    }
    else if (xkbDebugFlags & 0x1) {
        ErrorF
            ("[xkb] Rejecting client %d (0x%lx) (wants %d.%02d, have %d.%02d)\n",
             client->index, (long) client->clientAsMask, stuff->wantedMajor,
             stuff->wantedMinor, SERVER_XKB_MAJOR_VERSION,
             SERVER_XKB_MINOR_VERSION);
    }
    rep = (xkbUseExtensionReply) {
        .type = X_Reply,
        .supported = supported,
        .sequenceNumber = client->sequence,
        .length = 0,
        .serverMajor = SERVER_XKB_MAJOR_VERSION,
        .serverMinor = SERVER_XKB_MINOR_VERSION
    };
    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swaps(&rep.serverMajor);
        swaps(&rep.serverMinor);
    }
    WriteToClient(client, SIZEOF(xkbUseExtensionReply), &rep);
    return Success;
}