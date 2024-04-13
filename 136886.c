XkbSendGeometry(ClientPtr client,
                XkbGeometryPtr geom, xkbGetGeometryReply * rep, Bool freeGeom)
{
    char *desc, *start;
    int len;

    if (geom != NULL) {
        start = desc = xallocarray(rep->length, 4);
        if (!start)
            return BadAlloc;
        len = rep->length * 4;
        desc = XkbWriteCountedString(desc, geom->label_font, client->swapped);
        if (rep->nProperties > 0)
            desc = XkbWriteGeomProperties(desc, geom, client->swapped);
        if (rep->nColors > 0)
            desc = XkbWriteGeomColors(desc, geom, client->swapped);
        if (rep->nShapes > 0)
            desc = XkbWriteGeomShapes(desc, geom, client->swapped);
        if (rep->nSections > 0)
            desc = XkbWriteGeomSections(desc, geom, client->swapped);
        if (rep->nDoodads > 0)
            desc = XkbWriteGeomDoodads(desc, geom->num_doodads, geom->doodads,
                                       client->swapped);
        if (rep->nKeyAliases > 0)
            desc = XkbWriteGeomKeyAliases(desc, geom, client->swapped);
        if ((desc - start) != (len)) {
            ErrorF
                ("[xkb] BOGUS LENGTH in XkbSendGeometry, expected %d, got %ld\n",
                 len, (unsigned long) (desc - start));
        }
    }
    else {
        len = 0;
        start = NULL;
    }
    if (client->swapped) {
        swaps(&rep->sequenceNumber);
        swapl(&rep->length);
        swapl(&rep->name);
        swaps(&rep->widthMM);
        swaps(&rep->heightMM);
        swaps(&rep->nProperties);
        swaps(&rep->nColors);
        swaps(&rep->nShapes);
        swaps(&rep->nSections);
        swaps(&rep->nDoodads);
        swaps(&rep->nKeyAliases);
    }
    WriteToClient(client, SIZEOF(xkbGetGeometryReply), rep);
    if (len > 0)
        WriteToClient(client, len, start);
    if (start != NULL)
        free((char *) start);
    if (freeGeom)
        XkbFreeGeometry(geom, XkbGeomAllMask, TRUE);
    return Success;
}