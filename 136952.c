ProcXkbGetIndicatorState(ClientPtr client)
{
    xkbGetIndicatorStateReply rep;
    XkbSrvLedInfoPtr sli;
    DeviceIntPtr dev;

    REQUEST(xkbGetIndicatorStateReq);
    REQUEST_SIZE_MATCH(xkbGetIndicatorStateReq);

    if (!(client->xkbClientFlags & _XkbClientInitialized))
        return BadAccess;

    CHK_KBD_DEVICE(dev, stuff->deviceSpec, client, DixReadAccess);

    sli = XkbFindSrvLedInfo(dev, XkbDfltXIClass, XkbDfltXIId,
                            XkbXI_IndicatorStateMask);
    if (!sli)
        return BadAlloc;

    rep = (xkbGetIndicatorStateReply) {
        .type = X_Reply,
        .deviceID = dev->id,
        .sequenceNumber = client->sequence,
        .length = 0,
        .state = sli->effectiveState
    };

    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swapl(&rep.state);
    }
    WriteToClient(client, SIZEOF(xkbGetIndicatorStateReply), &rep);
    return Success;
}