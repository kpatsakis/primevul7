_XkbSetCompatMap(ClientPtr client, DeviceIntPtr dev,
                 xkbSetCompatMapReq * req, char *data, BOOL dryRun)
{
    XkbSrvInfoPtr xkbi;
    XkbDescPtr xkb;
    XkbCompatMapPtr compat;
    int nGroups;
    unsigned i, bit;

    xkbi = dev->key->xkbInfo;
    xkb = xkbi->desc;
    compat = xkb->compat;

    if ((req->nSI > 0) || (req->truncateSI)) {
        xkbSymInterpretWireDesc *wire;

        if (req->firstSI > compat->num_si) {
            client->errorValue = _XkbErrCode2(0x02, compat->num_si);
            return BadValue;
        }
        wire = (xkbSymInterpretWireDesc *) data;
        wire += req->nSI;
        data = (char *) wire;
    }

    nGroups = 0;
    if (req->groups != 0) {
        for (i = 0, bit = 1; i < XkbNumKbdGroups; i++, bit <<= 1) {
            if (req->groups & bit)
                nGroups++;
        }
    }
    data += nGroups * SIZEOF(xkbModsWireDesc);
    if (((data - ((char *) req)) / 4) != req->length) {
        return BadLength;
    }

    /* Done all the checks we can do */
    if (dryRun)
        return Success;

    data = (char *) &req[1];
    if (req->nSI > 0) {
        xkbSymInterpretWireDesc *wire = (xkbSymInterpretWireDesc *) data;
        XkbSymInterpretPtr sym;
        unsigned int skipped = 0;

        if ((unsigned) (req->firstSI + req->nSI) > compat->num_si) {
            compat->num_si = req->firstSI + req->nSI;
            compat->sym_interpret = reallocarray(compat->sym_interpret,
                                                 compat->num_si,
                                                 sizeof(XkbSymInterpretRec));
            if (!compat->sym_interpret) {
                compat->num_si = 0;
                return BadAlloc;
            }
        }
        else if (req->truncateSI) {
            compat->num_si = req->firstSI + req->nSI;
        }
        sym = &compat->sym_interpret[req->firstSI];
        for (i = 0; i < req->nSI; i++, wire++) {
            if (client->swapped) {
                swapl(&wire->sym);
            }
            if (wire->sym == NoSymbol && wire->match == XkbSI_AnyOfOrNone &&
                (wire->mods & 0xff) == 0xff &&
                wire->act.type == XkbSA_XFree86Private) {
                ErrorF("XKB: Skipping broken Any+AnyOfOrNone(All) -> Private "
                       "action from client\n");
                skipped++;
                continue;
            }
            sym->sym = wire->sym;
            sym->mods = wire->mods;
            sym->match = wire->match;
            sym->flags = wire->flags;
            sym->virtual_mod = wire->virtualMod;
            memcpy((char *) &sym->act, (char *) &wire->act,
                   SIZEOF(xkbActionWireDesc));
            sym++;
        }
        if (skipped) {
            if (req->firstSI + req->nSI < compat->num_si)
                memmove(sym, sym + skipped,
                        (compat->num_si - req->firstSI - req->nSI) *
                        sizeof(*sym));
            compat->num_si -= skipped;
        }
        data = (char *) wire;
    }
    else if (req->truncateSI) {
        compat->num_si = req->firstSI;
    }

    if (req->groups != 0) {
        xkbModsWireDesc *wire = (xkbModsWireDesc *) data;

        for (i = 0, bit = 1; i < XkbNumKbdGroups; i++, bit <<= 1) {
            if (req->groups & bit) {
                if (client->swapped) {
                    swaps(&wire->virtualMods);
                }
                compat->groups[i].mask = wire->realMods;
                compat->groups[i].real_mods = wire->realMods;
                compat->groups[i].vmods = wire->virtualMods;
                if (wire->virtualMods != 0) {
                    unsigned tmp;

                    tmp = XkbMaskForVMask(xkb, wire->virtualMods);
                    compat->groups[i].mask |= tmp;
                }
                data += SIZEOF(xkbModsWireDesc);
                wire = (xkbModsWireDesc *) data;
            }
        }
    }
    i = XkbPaddedSize((data - ((char *) req)));
    if ((i / 4) != req->length) {
        ErrorF("[xkb] Internal length error on read in _XkbSetCompatMap\n");
        return BadLength;
    }

    if (dev->xkb_interest) {
        xkbCompatMapNotify ev;

        ev.deviceID = dev->id;
        ev.changedGroups = req->groups;
        ev.firstSI = req->firstSI;
        ev.nSI = req->nSI;
        ev.nTotalSI = compat->num_si;
        XkbSendCompatMapNotify(dev, &ev);
    }

    if (req->recomputeActions) {
        XkbChangesRec change;
        unsigned check;
        XkbEventCauseRec cause;

        XkbSetCauseXkbReq(&cause, X_kbSetCompatMap, client);
        memset(&change, 0, sizeof(XkbChangesRec));
        XkbUpdateActions(dev, xkb->min_key_code, XkbNumKeys(xkb), &change,
                         &check, &cause);
        if (check)
            XkbCheckSecondaryEffects(xkbi, check, &change, &cause);
        XkbSendNotification(dev, &change, &cause);
    }
    return Success;
}