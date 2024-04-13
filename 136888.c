XkbSendCompatMap(ClientPtr client,
                 XkbCompatMapPtr compat, xkbGetCompatMapReply * rep)
{
    char *data;
    int size;

    if (rep->length > 0) {
        data = xallocarray(rep->length, 4);
        if (data) {
            register unsigned i, bit;
            xkbModsWireDesc *grp;
            XkbSymInterpretPtr sym = &compat->sym_interpret[rep->firstSI];
            xkbSymInterpretWireDesc *wire = (xkbSymInterpretWireDesc *) data;

            size = rep->length * 4;

            for (i = 0; i < rep->nSI; i++, sym++, wire++) {
                wire->sym = sym->sym;
                wire->mods = sym->mods;
                wire->match = sym->match;
                wire->virtualMod = sym->virtual_mod;
                wire->flags = sym->flags;
                memcpy((char *) &wire->act, (char *) &sym->act,
                       sz_xkbActionWireDesc);
                if (client->swapped) {
                    swapl(&wire->sym);
                }
            }
            if (rep->groups) {
                grp = (xkbModsWireDesc *) wire;
                for (i = 0, bit = 1; i < XkbNumKbdGroups; i++, bit <<= 1) {
                    if (rep->groups & bit) {
                        grp->mask = compat->groups[i].mask;
                        grp->realMods = compat->groups[i].real_mods;
                        grp->virtualMods = compat->groups[i].vmods;
                        if (client->swapped) {
                            swaps(&grp->virtualMods);
                        }
                        grp++;
                    }
                }
                wire = (xkbSymInterpretWireDesc *) grp;
            }
        }
        else
            return BadAlloc;
    }
    else
        data = NULL;

    if (client->swapped) {
        swaps(&rep->sequenceNumber);
        swapl(&rep->length);
        swaps(&rep->firstSI);
        swaps(&rep->nSI);
        swaps(&rep->nTotalSI);
    }

    WriteToClient(client, SIZEOF(xkbGetCompatMapReply), rep);
    if (data) {
        WriteToClient(client, size, data);
        free((char *) data);
    }
    return Success;
}