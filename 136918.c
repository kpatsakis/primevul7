ProcXkbListComponents(ClientPtr client)
{
    DeviceIntPtr dev;
    xkbListComponentsReply rep;
    unsigned len;
    unsigned char *str;
    uint8_t size;
    int i;

    REQUEST(xkbListComponentsReq);
    REQUEST_AT_LEAST_SIZE(xkbListComponentsReq);

    if (!(client->xkbClientFlags & _XkbClientInitialized))
        return BadAccess;

    CHK_KBD_DEVICE(dev, stuff->deviceSpec, client, DixGetAttrAccess);

    /* The request is followed by six Pascal strings (i.e. size in characters
     * followed by a string pattern) describing what the client wants us to
     * list.  We don't care, but might as well check they haven't got the
     * length wrong. */
    str = (unsigned char *) &stuff[1];
    for (i = 0; i < 6; i++) {
        size = *((uint8_t *)str);
        len = (str + size + 1) - ((unsigned char *) stuff);
        if ((XkbPaddedSize(len) / 4) > stuff->length)
            return BadLength;
        str += (size + 1);
    }
    if ((XkbPaddedSize(len) / 4) != stuff->length)
        return BadLength;
    rep = (xkbListComponentsReply) {
        .type = X_Reply,
        .deviceID = dev->id,
        .sequenceNumber = client->sequence,
        .length = 0,
        .nKeymaps = 0,
        .nKeycodes = 0,
        .nTypes = 0,
        .nCompatMaps = 0,
        .nSymbols = 0,
        .nGeometries = 0,
        .extra = 0
    };
    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swapl(&rep.length);
        swaps(&rep.nKeymaps);
        swaps(&rep.nKeycodes);
        swaps(&rep.nTypes);
        swaps(&rep.nCompatMaps);
        swaps(&rep.nSymbols);
        swaps(&rep.nGeometries);
        swaps(&rep.extra);
    }
    WriteToClient(client, SIZEOF(xkbListComponentsReply), &rep);
    return Success;
}