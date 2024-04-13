XkbWriteGeomSections(char *wire, XkbGeometryPtr geom, Bool swap)
{
    register int i;
    XkbSectionPtr section;
    xkbSectionWireDesc *sectionWire;

    for (i = 0, section = geom->sections; i < geom->num_sections;
         i++, section++) {
        sectionWire = (xkbSectionWireDesc *) wire;
        sectionWire->name = section->name;
        sectionWire->top = section->top;
        sectionWire->left = section->left;
        sectionWire->width = section->width;
        sectionWire->height = section->height;
        sectionWire->angle = section->angle;
        sectionWire->priority = section->priority;
        sectionWire->nRows = section->num_rows;
        sectionWire->nDoodads = section->num_doodads;
        sectionWire->nOverlays = section->num_overlays;
        sectionWire->pad = 0;
        if (swap) {
            swapl(&sectionWire->name);
            swaps(&sectionWire->top);
            swaps(&sectionWire->left);
            swaps(&sectionWire->width);
            swaps(&sectionWire->height);
            swaps(&sectionWire->angle);
        }
        wire = (char *) &sectionWire[1];
        if (section->rows) {
            int r;
            XkbRowPtr row;
            xkbRowWireDesc *rowWire;

            for (r = 0, row = section->rows; r < section->num_rows; r++, row++) {
                rowWire = (xkbRowWireDesc *) wire;
                rowWire->top = row->top;
                rowWire->left = row->left;
                rowWire->nKeys = row->num_keys;
                rowWire->vertical = row->vertical;
                rowWire->pad = 0;
                if (swap) {
                    swaps(&rowWire->top);
                    swaps(&rowWire->left);
                }
                wire = (char *) &rowWire[1];
                if (row->keys) {
                    int k;
                    XkbKeyPtr key;
                    xkbKeyWireDesc *keyWire;

                    keyWire = (xkbKeyWireDesc *) wire;
                    for (k = 0, key = row->keys; k < row->num_keys; k++, key++) {
                        memcpy(keyWire[k].name, key->name.name,
                               XkbKeyNameLength);
                        keyWire[k].gap = key->gap;
                        keyWire[k].shapeNdx = key->shape_ndx;
                        keyWire[k].colorNdx = key->color_ndx;
                        if (swap) {
                            swaps(&keyWire[k].gap);
                        }
                    }
                    wire = (char *) &keyWire[row->num_keys];
                }
            }
        }
        if (section->doodads) {
            wire = XkbWriteGeomDoodads(wire,
                                       section->num_doodads, section->doodads,
                                       swap);
        }
        if (section->overlays) {
            register int o;

            for (o = 0; o < section->num_overlays; o++) {
                wire = XkbWriteGeomOverlay(wire, &section->overlays[o], swap);
            }
        }
    }
    return wire;
}