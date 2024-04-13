XkbSendIndicatorMap(ClientPtr client,
                    XkbIndicatorPtr indicators, xkbGetIndicatorMapReply * rep)
{
    int length;
    CARD8 *map;
    register int i;
    register unsigned bit;

    if (rep->length > 0) {
        CARD8 *to;

        to = map = xallocarray(rep->length, 4);
        if (map) {
            xkbIndicatorMapWireDesc *wire = (xkbIndicatorMapWireDesc *) to;

            length = rep->length * 4;

            for (i = 0, bit = 1; i < XkbNumIndicators; i++, bit <<= 1) {
                if (rep->which & bit) {
                    wire->flags = indicators->maps[i].flags;
                    wire->whichGroups = indicators->maps[i].which_groups;
                    wire->groups = indicators->maps[i].groups;
                    wire->whichMods = indicators->maps[i].which_mods;
                    wire->mods = indicators->maps[i].mods.mask;
                    wire->realMods = indicators->maps[i].mods.real_mods;
                    wire->virtualMods = indicators->maps[i].mods.vmods;
                    wire->ctrls = indicators->maps[i].ctrls;
                    if (client->swapped) {
                        swaps(&wire->virtualMods);
                        swapl(&wire->ctrls);
                    }
                    wire++;
                }
            }
            to = (CARD8 *) wire;
            if ((to - map) != length) {
                client->errorValue = _XkbErrCode2(0xff, length);
                free(map);
                return BadLength;
            }
        }
        else
            return BadAlloc;
    }
    else
        map = NULL;
    if (client->swapped) {
        swaps(&rep->sequenceNumber);
        swapl(&rep->length);
        swapl(&rep->which);
        swapl(&rep->realIndicators);
    }
    WriteToClient(client, SIZEOF(xkbGetIndicatorMapReply), rep);
    if (map) {
        WriteToClient(client, length, map);
        free((char *) map);
    }
    return Success;
}