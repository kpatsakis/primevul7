SendDeviceLedInfo(XkbSrvLedInfoPtr sli, ClientPtr client)
{
    xkbDeviceLedsWireDesc wire;
    int length;

    length = 0;
    wire.ledClass = sli->class;
    wire.ledID = sli->id;
    wire.namesPresent = sli->namesPresent;
    wire.mapsPresent = sli->mapsPresent;
    wire.physIndicators = sli->physIndicators;
    wire.state = sli->effectiveState;
    if (client->swapped) {
        swaps(&wire.ledClass);
        swaps(&wire.ledID);
        swapl(&wire.namesPresent);
        swapl(&wire.mapsPresent);
        swapl(&wire.physIndicators);
        swapl(&wire.state);
    }
    WriteToClient(client, SIZEOF(xkbDeviceLedsWireDesc), &wire);
    length += SIZEOF(xkbDeviceLedsWireDesc);
    if (sli->namesPresent | sli->mapsPresent) {
        register unsigned i, bit;

        if (sli->namesPresent) {
            CARD32 awire;

            for (i = 0, bit = 1; i < XkbNumIndicators; i++, bit <<= 1) {
                if (sli->namesPresent & bit) {
                    awire = (CARD32) sli->names[i];
                    if (client->swapped) {
                        swapl(&awire);
                    }
                    WriteToClient(client, 4, &awire);
                    length += 4;
                }
            }
        }
        if (sli->mapsPresent) {
            for (i = 0, bit = 1; i < XkbNumIndicators; i++, bit <<= 1) {
                xkbIndicatorMapWireDesc iwire;

                if (sli->mapsPresent & bit) {
                    iwire.flags = sli->maps[i].flags;
                    iwire.whichGroups = sli->maps[i].which_groups;
                    iwire.groups = sli->maps[i].groups;
                    iwire.whichMods = sli->maps[i].which_mods;
                    iwire.mods = sli->maps[i].mods.mask;
                    iwire.realMods = sli->maps[i].mods.real_mods;
                    iwire.virtualMods = sli->maps[i].mods.vmods;
                    iwire.ctrls = sli->maps[i].ctrls;
                    if (client->swapped) {
                        swaps(&iwire.virtualMods);
                        swapl(&iwire.ctrls);
                    }
                    WriteToClient(client, SIZEOF(xkbIndicatorMapWireDesc),
                                  &iwire);
                    length += SIZEOF(xkbIndicatorMapWireDesc);
                }
            }
        }
    }
    return length;
}