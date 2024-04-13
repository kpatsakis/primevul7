ProcXkbGetState(ClientPtr client)
{
    REQUEST(xkbGetStateReq);
    DeviceIntPtr dev;
    xkbGetStateReply rep;
    XkbStateRec *xkb;

    REQUEST_SIZE_MATCH(xkbGetStateReq);

    if (!(client->xkbClientFlags & _XkbClientInitialized))
        return BadAccess;

    CHK_KBD_DEVICE(dev, stuff->deviceSpec, client, DixGetAttrAccess);

    xkb = &dev->key->xkbInfo->state;
    rep = (xkbGetStateReply) {
        .type = X_Reply,
        .deviceID = dev->id,
        .sequenceNumber = client->sequence,
        .length = 0,
        .mods = XkbStateFieldFromRec(xkb) & 0xff,
        .baseMods = xkb->base_mods,
        .latchedMods = xkb->latched_mods,
        .lockedMods = xkb->locked_mods,
        .group = xkb->group,
        .lockedGroup = xkb->locked_group,
        .baseGroup = xkb->base_group,
        .latchedGroup = xkb->latched_group,
        .compatState = xkb->compat_state,
        .ptrBtnState = xkb->ptr_buttons
    };
    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swaps(&rep.ptrBtnState);
    }
    WriteToClient(client, SIZEOF(xkbGetStateReply), &rep);
    return Success;
}