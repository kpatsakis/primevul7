ProcXkbGetControls(ClientPtr client)
{
    xkbGetControlsReply rep;
    XkbControlsPtr xkb;
    DeviceIntPtr dev;

    REQUEST(xkbGetControlsReq);
    REQUEST_SIZE_MATCH(xkbGetControlsReq);

    if (!(client->xkbClientFlags & _XkbClientInitialized))
        return BadAccess;

    CHK_KBD_DEVICE(dev, stuff->deviceSpec, client, DixGetAttrAccess);

    xkb = dev->key->xkbInfo->desc->ctrls;
    rep = (xkbGetControlsReply) {
        .type = X_Reply,
        .deviceID = ((DeviceIntPtr) dev)->id,
        .sequenceNumber = client->sequence,
        .length = bytes_to_int32(SIZEOF(xkbGetControlsReply) -
                                 SIZEOF(xGenericReply)),
        .mkDfltBtn = xkb->mk_dflt_btn,
        .numGroups = xkb->num_groups,
        .groupsWrap = xkb->groups_wrap,
        .internalMods = xkb->internal.mask,
        .ignoreLockMods = xkb->ignore_lock.mask,
        .internalRealMods = xkb->internal.real_mods,
        .ignoreLockRealMods = xkb->ignore_lock.real_mods,
        .internalVMods = xkb->internal.vmods,
        .ignoreLockVMods = xkb->ignore_lock.vmods,
        .repeatDelay = xkb->repeat_delay,
        .repeatInterval = xkb->repeat_interval,
        .slowKeysDelay = xkb->slow_keys_delay,
        .debounceDelay = xkb->debounce_delay,
        .mkDelay = xkb->mk_delay,
        .mkInterval = xkb->mk_interval,
        .mkTimeToMax = xkb->mk_time_to_max,
        .mkMaxSpeed = xkb->mk_max_speed,
        .mkCurve = xkb->mk_curve,
        .axOptions = xkb->ax_options,
        .axTimeout = xkb->ax_timeout,
        .axtOptsMask = xkb->axt_opts_mask,
        .axtOptsValues = xkb->axt_opts_values,
        .axtCtrlsMask = xkb->axt_ctrls_mask,
        .axtCtrlsValues = xkb->axt_ctrls_values,
        .enabledCtrls = xkb->enabled_ctrls,
    };
    memcpy(rep.perKeyRepeat, xkb->per_key_repeat, XkbPerKeyBitArraySize);
    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swapl(&rep.length);
        swaps(&rep.internalVMods);
        swaps(&rep.ignoreLockVMods);
        swapl(&rep.enabledCtrls);
        swaps(&rep.repeatDelay);
        swaps(&rep.repeatInterval);
        swaps(&rep.slowKeysDelay);
        swaps(&rep.debounceDelay);
        swaps(&rep.mkDelay);
        swaps(&rep.mkInterval);
        swaps(&rep.mkTimeToMax);
        swaps(&rep.mkMaxSpeed);
        swaps(&rep.mkCurve);
        swaps(&rep.axTimeout);
        swapl(&rep.axtCtrlsMask);
        swapl(&rep.axtCtrlsValues);
        swaps(&rep.axtOptsMask);
        swaps(&rep.axtOptsValues);
        swaps(&rep.axOptions);
    }
    WriteToClient(client, SIZEOF(xkbGetControlsReply), &rep);
    return Success;
}