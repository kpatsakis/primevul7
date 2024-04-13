XkbWriteKeyTypes(XkbDescPtr xkb,
                 xkbGetMapReply * rep, char *buf, ClientPtr client)
{
    XkbKeyTypePtr type;
    unsigned i;
    xkbKeyTypeWireDesc *wire;

    type = &xkb->map->types[rep->firstType];
    for (i = 0; i < rep->nTypes; i++, type++) {
        register unsigned n;

        wire = (xkbKeyTypeWireDesc *) buf;
        wire->mask = type->mods.mask;
        wire->realMods = type->mods.real_mods;
        wire->virtualMods = type->mods.vmods;
        wire->numLevels = type->num_levels;
        wire->nMapEntries = type->map_count;
        wire->preserve = (type->preserve != NULL);
        if (client->swapped) {
            swaps(&wire->virtualMods);
        }

        buf = (char *) &wire[1];
        if (wire->nMapEntries > 0) {
            xkbKTMapEntryWireDesc *ewire;
            XkbKTMapEntryPtr entry;

            ewire = (xkbKTMapEntryWireDesc *) buf;
            entry = type->map;
            for (n = 0; n < type->map_count; n++, ewire++, entry++) {
                ewire->active = entry->active;
                ewire->mask = entry->mods.mask;
                ewire->level = entry->level;
                ewire->realMods = entry->mods.real_mods;
                ewire->virtualMods = entry->mods.vmods;
                if (client->swapped) {
                    swaps(&ewire->virtualMods);
                }
            }
            buf = (char *) ewire;
            if (type->preserve != NULL) {
                xkbModsWireDesc *pwire;
                XkbModsPtr preserve;

                pwire = (xkbModsWireDesc *) buf;
                preserve = type->preserve;
                for (n = 0; n < type->map_count; n++, pwire++, preserve++) {
                    pwire->mask = preserve->mask;
                    pwire->realMods = preserve->real_mods;
                    pwire->virtualMods = preserve->vmods;
                    if (client->swapped) {
                        swaps(&pwire->virtualMods);
                    }
                }
                buf = (char *) pwire;
            }
        }
    }
    return buf;
}