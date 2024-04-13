_XkbSetMapChecks(ClientPtr client, DeviceIntPtr dev, xkbSetMapReq * req,
                 char *values, Bool doswap)
{
    XkbSrvInfoPtr xkbi;
    XkbDescPtr xkb;
    int error;
    int nTypes = 0, nActions;
    CARD8 mapWidths[XkbMaxLegalKeyCode + 1] = { 0 };
    CARD16 symsPerKey[XkbMaxLegalKeyCode + 1] = { 0 };
    XkbSymMapPtr map;
    int i;

    if (!dev->key)
        return 0;

    xkbi = dev->key->xkbInfo;
    xkb = xkbi->desc;

    if ((xkb->min_key_code != req->minKeyCode) ||
        (xkb->max_key_code != req->maxKeyCode)) {
        if (client->xkbClientFlags & _XkbClientIsAncient) {
            /* pre 1.0 versions of Xlib have a bug */
            req->minKeyCode = xkb->min_key_code;
            req->maxKeyCode = xkb->max_key_code;
        }
        else {
            if (!XkbIsLegalKeycode(req->minKeyCode)) {
                client->errorValue =
                    _XkbErrCode3(2, req->minKeyCode, req->maxKeyCode);
                return BadValue;
            }
            if (req->minKeyCode > req->maxKeyCode) {
                client->errorValue =
                    _XkbErrCode3(3, req->minKeyCode, req->maxKeyCode);
                return BadMatch;
            }
        }
    }

    if ((req->present & XkbKeyTypesMask) &&
        (!CheckKeyTypes(client, xkb, req, (xkbKeyTypeWireDesc **) &values,
                        &nTypes, mapWidths, doswap))) {
        client->errorValue = nTypes;
        return BadValue;
    }
    else {
        nTypes = xkb->map->num_types;
    }

    /* symsPerKey/mapWidths must be filled regardless of client-side flags */
    map = &xkb->map->key_sym_map[xkb->min_key_code];
    for (i = xkb->min_key_code; i < xkb->max_key_code; i++, map++) {
        register int g, ng, w;

        ng = XkbNumGroups(map->group_info);
        for (w = g = 0; g < ng; g++) {
            if (map->kt_index[g] >= (unsigned) nTypes) {
                client->errorValue = _XkbErrCode4(0x13, i, g, map->kt_index[g]);
                return BadValue;
            }
            if (mapWidths[map->kt_index[g]] > w)
                w = mapWidths[map->kt_index[g]];
        }
        symsPerKey[i] = w * ng;
    }

    if ((req->present & XkbKeySymsMask) &&
        (!CheckKeySyms(client, xkb, req, nTypes, mapWidths, symsPerKey,
                       (xkbSymMapWireDesc **) &values, &error, doswap))) {
        client->errorValue = error;
        return BadValue;
    }

    if ((req->present & XkbKeyActionsMask) &&
        (!CheckKeyActions(xkb, req, nTypes, mapWidths, symsPerKey,
                          (CARD8 **) &values, &nActions))) {
        client->errorValue = nActions;
        return BadValue;
    }

    if ((req->present & XkbKeyBehaviorsMask) &&
        (!CheckKeyBehaviors
         (xkb, req, (xkbBehaviorWireDesc **) &values, &error))) {
        client->errorValue = error;
        return BadValue;
    }

    if ((req->present & XkbVirtualModsMask) &&
        (!CheckVirtualMods(xkb, req, (CARD8 **) &values, &error))) {
        client->errorValue = error;
        return BadValue;
    }
    if ((req->present & XkbExplicitComponentsMask) &&
        (!CheckKeyExplicit(xkb, req, (CARD8 **) &values, &error))) {
        client->errorValue = error;
        return BadValue;
    }
    if ((req->present & XkbModifierMapMask) &&
        (!CheckModifierMap(xkb, req, (CARD8 **) &values, &error))) {
        client->errorValue = error;
        return BadValue;
    }
    if ((req->present & XkbVirtualModMapMask) &&
        (!CheckVirtualModMap
         (xkb, req, (xkbVModMapWireDesc **) &values, &error))) {
        client->errorValue = error;
        return BadValue;
    }

    if (((values - ((char *) req)) / 4) != req->length) {
        ErrorF("[xkb] Internal error! Bad length in XkbSetMap (after check)\n");
        client->errorValue = values - ((char *) &req[1]);
        return BadLength;
    }

    return Success;
}