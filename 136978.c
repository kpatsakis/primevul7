ProcXkbGetDeviceInfo(ClientPtr client)
{
    DeviceIntPtr dev;
    xkbGetDeviceInfoReply rep;
    int status, nDeviceLedFBs;
    unsigned length, nameLen;
    CARD16 ledClass, ledID;
    unsigned wanted;
    char *str;

    REQUEST(xkbGetDeviceInfoReq);
    REQUEST_SIZE_MATCH(xkbGetDeviceInfoReq);

    if (!(client->xkbClientFlags & _XkbClientInitialized))
        return BadAccess;

    wanted = stuff->wanted;

    CHK_ANY_DEVICE(dev, stuff->deviceSpec, client, DixGetAttrAccess);
    CHK_MASK_LEGAL(0x01, wanted, XkbXI_AllDeviceFeaturesMask);

    if ((!dev->button) || ((stuff->nBtns < 1) && (!stuff->allBtns)))
        wanted &= ~XkbXI_ButtonActionsMask;
    if ((!dev->kbdfeed) && (!dev->leds))
        wanted &= ~XkbXI_IndicatorsMask;

    nameLen = XkbSizeCountedString(dev->name);
    rep = (xkbGetDeviceInfoReply) {
        .type = X_Reply,
        .deviceID = dev->id,
        .sequenceNumber = client->sequence,
        .length = nameLen / 4,
        .present = wanted,
        .supported = XkbXI_AllDeviceFeaturesMask,
        .unsupported = 0,
        .nDeviceLedFBs = 0,
        .firstBtnWanted = 0,
        .nBtnsWanted = 0,
        .firstBtnRtrn = 0,
        .nBtnsRtrn = 0,
        .totalBtns = dev->button ? dev->button->numButtons : 0,
        .hasOwnState = (dev->key && dev->key->xkbInfo),
        .dfltKbdFB = dev->kbdfeed ? dev->kbdfeed->ctrl.id : XkbXINone,
        .dfltLedFB = dev->leds ? dev->leds->ctrl.id : XkbXINone,
        .devType = dev->xinput_type
    };

    ledClass = stuff->ledClass;
    ledID = stuff->ledID;

    if (wanted & XkbXI_ButtonActionsMask) {
        if (stuff->allBtns) {
            stuff->firstBtn = 0;
            stuff->nBtns = dev->button->numButtons;
        }

        if ((stuff->firstBtn + stuff->nBtns) > dev->button->numButtons) {
            client->errorValue = _XkbErrCode4(0x02, dev->button->numButtons,
                                              stuff->firstBtn, stuff->nBtns);
            return BadValue;
        }
        else {
            rep.firstBtnWanted = stuff->firstBtn;
            rep.nBtnsWanted = stuff->nBtns;
            if (dev->button->xkb_acts != NULL) {
                XkbAction *act;
                register int i;

                rep.firstBtnRtrn = stuff->firstBtn;
                rep.nBtnsRtrn = stuff->nBtns;
                act = &dev->button->xkb_acts[rep.firstBtnWanted];
                for (i = 0; i < rep.nBtnsRtrn; i++, act++) {
                    if (act->type != XkbSA_NoAction)
                        break;
                }
                rep.firstBtnRtrn += i;
                rep.nBtnsRtrn -= i;
                act =
                    &dev->button->xkb_acts[rep.firstBtnRtrn + rep.nBtnsRtrn -
                                           1];
                for (i = 0; i < rep.nBtnsRtrn; i++, act--) {
                    if (act->type != XkbSA_NoAction)
                        break;
                }
                rep.nBtnsRtrn -= i;
            }
            rep.length += (rep.nBtnsRtrn * SIZEOF(xkbActionWireDesc)) / 4;
        }
    }

    if (wanted & XkbXI_IndicatorsMask) {
        status = CheckDeviceLedFBs(dev, ledClass, ledID, &rep, client);
        if (status != Success)
            return status;
    }
    length = rep.length * 4;
    nDeviceLedFBs = rep.nDeviceLedFBs;
    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swapl(&rep.length);
        swaps(&rep.present);
        swaps(&rep.supported);
        swaps(&rep.unsupported);
        swaps(&rep.nDeviceLedFBs);
        swaps(&rep.dfltKbdFB);
        swaps(&rep.dfltLedFB);
        swapl(&rep.devType);
    }
    WriteToClient(client, SIZEOF(xkbGetDeviceInfoReply), &rep);

    str = malloc(nameLen);
    if (!str)
        return BadAlloc;
    XkbWriteCountedString(str, dev->name, client->swapped);
    WriteToClient(client, nameLen, str);
    free(str);
    length -= nameLen;

    if (rep.nBtnsRtrn > 0) {
        int sz;
        xkbActionWireDesc *awire;

        sz = rep.nBtnsRtrn * SIZEOF(xkbActionWireDesc);
        awire = (xkbActionWireDesc *) &dev->button->xkb_acts[rep.firstBtnRtrn];
        WriteToClient(client, sz, awire);
        length -= sz;
    }
    if (nDeviceLedFBs > 0) {
        status = SendDeviceLedFBs(dev, ledClass, ledID, length, client);
        if (status != Success)
            return status;
    }
    else if (length != 0) {
        ErrorF("[xkb] Internal Error!  BadLength in ProcXkbGetDeviceInfo\n");
        ErrorF("[xkb]                  Wrote %d fewer bytes than expected\n",
               length);
        return BadLength;
    }
    return Success;
}