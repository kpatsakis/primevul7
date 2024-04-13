ProcXkbSetDebuggingFlags(ClientPtr client)
{
    CARD32 newFlags, newCtrls, extraLength;
    xkbSetDebuggingFlagsReply rep;
    int rc;

    REQUEST(xkbSetDebuggingFlagsReq);
    REQUEST_AT_LEAST_SIZE(xkbSetDebuggingFlagsReq);

    rc = XaceHook(XACE_SERVER_ACCESS, client, DixDebugAccess);
    if (rc != Success)
        return rc;

    newFlags = xkbDebugFlags & (~stuff->affectFlags);
    newFlags |= (stuff->flags & stuff->affectFlags);
    newCtrls = xkbDebugCtrls & (~stuff->affectCtrls);
    newCtrls |= (stuff->ctrls & stuff->affectCtrls);
    if (xkbDebugFlags || newFlags || stuff->msgLength) {
        ErrorF("[xkb] XkbDebug: Setting debug flags to 0x%lx\n",
               (long) newFlags);
        if (newCtrls != xkbDebugCtrls)
            ErrorF("[xkb] XkbDebug: Setting debug controls to 0x%lx\n",
                   (long) newCtrls);
    }
    extraLength = (stuff->length << 2) - sz_xkbSetDebuggingFlagsReq;
    if (stuff->msgLength > 0) {
        char *msg;

        if (extraLength < XkbPaddedSize(stuff->msgLength)) {
            ErrorF
                ("[xkb] XkbDebug: msgLength= %d, length= %ld (should be %d)\n",
                 stuff->msgLength, (long) extraLength,
                 XkbPaddedSize(stuff->msgLength));
            return BadLength;
        }
        msg = (char *) &stuff[1];
        if (msg[stuff->msgLength - 1] != '\0') {
            ErrorF("[xkb] XkbDebug: message not null-terminated\n");
            return BadValue;
        }
        ErrorF("[xkb] XkbDebug: %s\n", msg);
    }
    xkbDebugFlags = newFlags;
    xkbDebugCtrls = newCtrls;

    rep = (xkbSetDebuggingFlagsReply) {
        .type = X_Reply,
        .sequenceNumber = client->sequence,
        .length = 0,
        .currentFlags = newFlags,
        .currentCtrls = newCtrls,
        .supportedFlags = ~0,
        .supportedCtrls = ~0
    };
    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swapl(&rep.currentFlags);
        swapl(&rep.currentCtrls);
        swapl(&rep.supportedFlags);
        swapl(&rep.supportedCtrls);
    }
    WriteToClient(client, SIZEOF(xkbSetDebuggingFlagsReply), &rep);
    return Success;
}