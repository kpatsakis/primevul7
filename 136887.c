ProcXkbPerClientFlags(ClientPtr client)
{
    DeviceIntPtr dev;
    xkbPerClientFlagsReply rep;
    XkbInterestPtr interest;
    Mask access_mode = DixGetAttrAccess | DixSetAttrAccess;

    REQUEST(xkbPerClientFlagsReq);
    REQUEST_SIZE_MATCH(xkbPerClientFlagsReq);

    if (!(client->xkbClientFlags & _XkbClientInitialized))
        return BadAccess;

    CHK_KBD_DEVICE(dev, stuff->deviceSpec, client, access_mode);
    CHK_MASK_LEGAL(0x01, stuff->change, XkbPCF_AllFlagsMask);
    CHK_MASK_MATCH(0x02, stuff->change, stuff->value);

    interest = XkbFindClientResource((DevicePtr) dev, client);
    if (stuff->change) {
        client->xkbClientFlags &= ~stuff->change;
        client->xkbClientFlags |= stuff->value;
    }
    if (stuff->change & XkbPCF_AutoResetControlsMask) {
        Bool want;

        want = stuff->value & XkbPCF_AutoResetControlsMask;
        if (interest && !want) {
            interest->autoCtrls = interest->autoCtrlValues = 0;
        }
        else if (want && (!interest)) {
            XID id = FakeClientID(client->index);

            if (!AddResource(id, RT_XKBCLIENT, dev))
                return BadAlloc;
            interest = XkbAddClientResource((DevicePtr) dev, client, id);
            if (!interest)
                return BadAlloc;
        }
        if (interest && want) {
            register unsigned affect;

            affect = stuff->ctrlsToChange;

            CHK_MASK_LEGAL(0x03, affect, XkbAllBooleanCtrlsMask);
            CHK_MASK_MATCH(0x04, affect, stuff->autoCtrls);
            CHK_MASK_MATCH(0x05, stuff->autoCtrls, stuff->autoCtrlValues);

            interest->autoCtrls &= ~affect;
            interest->autoCtrlValues &= ~affect;
            interest->autoCtrls |= stuff->autoCtrls & affect;
            interest->autoCtrlValues |= stuff->autoCtrlValues & affect;
        }
    }

    rep = (xkbPerClientFlagsReply) {
        .type = X_Reply,
        .sequenceNumber = client->sequence,
        .length = 0,
        .supported = XkbPCF_AllFlagsMask,
        .value = client->xkbClientFlags & XkbPCF_AllFlagsMask,
        .autoCtrls = interest ? interest->autoCtrls : 0,
        .autoCtrlValues =  interest ? interest->autoCtrlValues : 0,
    };
    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swapl(&rep.supported);
        swapl(&rep.value);
        swapl(&rep.autoCtrls);
        swapl(&rep.autoCtrlValues);
    }
    WriteToClient(client, SIZEOF(xkbPerClientFlagsReply), &rep);
    return Success;
}