ProcXkbGetGeometry(ClientPtr client)
{
    DeviceIntPtr dev;
    xkbGetGeometryReply rep;
    XkbGeometryPtr geom;
    Bool shouldFree;
    Status status;

    REQUEST(xkbGetGeometryReq);
    REQUEST_SIZE_MATCH(xkbGetGeometryReq);

    if (!(client->xkbClientFlags & _XkbClientInitialized))
        return BadAccess;

    CHK_KBD_DEVICE(dev, stuff->deviceSpec, client, DixGetAttrAccess);
    CHK_ATOM_OR_NONE(stuff->name);

    geom = XkbLookupNamedGeometry(dev, stuff->name, &shouldFree);
    rep = (xkbGetGeometryReply) {
        .type = X_Reply,
        .deviceID = dev->id,
        .sequenceNumber = client->sequence,
        .length = 0
    };
    status = XkbComputeGetGeometryReplySize(geom, &rep, stuff->name);
    if (status != Success)
        return status;
    else
        return XkbSendGeometry(client, geom, &rep, shouldFree);
}