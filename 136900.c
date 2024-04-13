ProcXkbGetIndicatorMap(ClientPtr client)
{
    xkbGetIndicatorMapReply rep;
    DeviceIntPtr dev;
    XkbDescPtr xkb;
    XkbIndicatorPtr leds;

    REQUEST(xkbGetIndicatorMapReq);
    REQUEST_SIZE_MATCH(xkbGetIndicatorMapReq);

    if (!(client->xkbClientFlags & _XkbClientInitialized))
        return BadAccess;

    CHK_KBD_DEVICE(dev, stuff->deviceSpec, client, DixGetAttrAccess);

    xkb = dev->key->xkbInfo->desc;
    leds = xkb->indicators;

    rep = (xkbGetIndicatorMapReply) {
        .type = X_Reply,
        .deviceID = dev->id,
        .sequenceNumber = client->sequence,
        .length = 0,
        .which = stuff->which
    };
    XkbComputeGetIndicatorMapReplySize(leds, &rep);
    return XkbSendIndicatorMap(client, leds, &rep);
}