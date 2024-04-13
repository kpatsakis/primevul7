ProcXkbGetNamedIndicator(ClientPtr client)
{
    DeviceIntPtr dev;
    xkbGetNamedIndicatorReply rep;
    register int i = 0;
    XkbSrvLedInfoPtr sli;
    XkbIndicatorMapPtr map = NULL;

    REQUEST(xkbGetNamedIndicatorReq);
    REQUEST_SIZE_MATCH(xkbGetNamedIndicatorReq);

    if (!(client->xkbClientFlags & _XkbClientInitialized))
        return BadAccess;

    CHK_LED_DEVICE(dev, stuff->deviceSpec, client, DixReadAccess);
    CHK_ATOM_ONLY(stuff->indicator);

    sli = XkbFindSrvLedInfo(dev, stuff->ledClass, stuff->ledID, 0);
    if (!sli)
        return BadAlloc;

    i = 0;
    map = NULL;
    if ((sli->names) && (sli->maps)) {
        for (i = 0; i < XkbNumIndicators; i++) {
            if (stuff->indicator == sli->names[i]) {
                map = &sli->maps[i];
                break;
            }
        }
    }

    rep = (xkbGetNamedIndicatorReply) {
        .type = X_Reply,
        .sequenceNumber = client->sequence,
        .length = 0,
        .deviceID = dev->id,
        .indicator = stuff->indicator
    };
    if (map != NULL) {
        rep.found = TRUE;
        rep.on = ((sli->effectiveState & (1 << i)) != 0);
        rep.realIndicator = ((sli->physIndicators & (1 << i)) != 0);
        rep.ndx = i;
        rep.flags = map->flags;
        rep.whichGroups = map->which_groups;
        rep.groups = map->groups;
        rep.whichMods = map->which_mods;
        rep.mods = map->mods.mask;
        rep.realMods = map->mods.real_mods;
        rep.virtualMods = map->mods.vmods;
        rep.ctrls = map->ctrls;
        rep.supported = TRUE;
    }
    else {
        rep.found = FALSE;
        rep.on = FALSE;
        rep.realIndicator = FALSE;
        rep.ndx = XkbNoIndicator;
        rep.flags = 0;
        rep.whichGroups = 0;
        rep.groups = 0;
        rep.whichMods = 0;
        rep.mods = 0;
        rep.realMods = 0;
        rep.virtualMods = 0;
        rep.ctrls = 0;
        rep.supported = TRUE;
    }
    if (client->swapped) {
        swapl(&rep.length);
        swaps(&rep.sequenceNumber);
        swapl(&rep.indicator);
        swaps(&rep.virtualMods);
        swapl(&rep.ctrls);
    }

    WriteToClient(client, SIZEOF(xkbGetNamedIndicatorReply), &rep);
    return Success;
}