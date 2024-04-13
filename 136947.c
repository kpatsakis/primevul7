ProcXkbGetNames(ClientPtr client)
{
    DeviceIntPtr dev;
    XkbDescPtr xkb;
    xkbGetNamesReply rep;

    REQUEST(xkbGetNamesReq);
    REQUEST_SIZE_MATCH(xkbGetNamesReq);

    if (!(client->xkbClientFlags & _XkbClientInitialized))
        return BadAccess;

    CHK_KBD_DEVICE(dev, stuff->deviceSpec, client, DixGetAttrAccess);
    CHK_MASK_LEGAL(0x01, stuff->which, XkbAllNamesMask);

    xkb = dev->key->xkbInfo->desc;
    rep = (xkbGetNamesReply) {
        .type = X_Reply,
        .deviceID = dev->id,
        .sequenceNumber = client->sequence,
        .length = 0,
        .which = stuff->which,
        .nTypes = xkb->map->num_types,
        .firstKey = xkb->min_key_code,
        .nKeys = XkbNumKeys(xkb),
        .nKeyAliases = xkb->names ? xkb->names->num_key_aliases : 0,
        .nRadioGroups = xkb->names ? xkb->names->num_rg : 0
    };
    XkbComputeGetNamesReplySize(xkb, &rep);
    return XkbSendNames(client, xkb, &rep);
}