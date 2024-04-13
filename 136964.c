XkbWriteGeomDoodads(char *wire, int num_doodads, XkbDoodadPtr doodad, Bool swap)
{
    register int i;
    xkbDoodadWireDesc *doodadWire;

    for (i = 0; i < num_doodads; i++, doodad++) {
        doodadWire = (xkbDoodadWireDesc *) wire;
        wire = (char *) &doodadWire[1];
        memset(doodadWire, 0, SIZEOF(xkbDoodadWireDesc));
        doodadWire->any.name = doodad->any.name;
        doodadWire->any.type = doodad->any.type;
        doodadWire->any.priority = doodad->any.priority;
        doodadWire->any.top = doodad->any.top;
        doodadWire->any.left = doodad->any.left;
        if (swap) {
            swapl(&doodadWire->any.name);
            swaps(&doodadWire->any.top);
            swaps(&doodadWire->any.left);
        }
        switch (doodad->any.type) {
        case XkbOutlineDoodad:
        case XkbSolidDoodad:
            doodadWire->shape.angle = doodad->shape.angle;
            doodadWire->shape.colorNdx = doodad->shape.color_ndx;
            doodadWire->shape.shapeNdx = doodad->shape.shape_ndx;
            if (swap) {
                swaps(&doodadWire->shape.angle);
            }
            break;
        case XkbTextDoodad:
            doodadWire->text.angle = doodad->text.angle;
            doodadWire->text.width = doodad->text.width;
            doodadWire->text.height = doodad->text.height;
            doodadWire->text.colorNdx = doodad->text.color_ndx;
            if (swap) {
                swaps(&doodadWire->text.angle);
                swaps(&doodadWire->text.width);
                swaps(&doodadWire->text.height);
            }
            wire = XkbWriteCountedString(wire, doodad->text.text, swap);
            wire = XkbWriteCountedString(wire, doodad->text.font, swap);
            break;
        case XkbIndicatorDoodad:
            doodadWire->indicator.shapeNdx = doodad->indicator.shape_ndx;
            doodadWire->indicator.onColorNdx = doodad->indicator.on_color_ndx;
            doodadWire->indicator.offColorNdx = doodad->indicator.off_color_ndx;
            break;
        case XkbLogoDoodad:
            doodadWire->logo.angle = doodad->logo.angle;
            doodadWire->logo.colorNdx = doodad->logo.color_ndx;
            doodadWire->logo.shapeNdx = doodad->logo.shape_ndx;
            wire = XkbWriteCountedString(wire, doodad->logo.logo_name, swap);
            break;
        default:
            ErrorF("[xkb] Unknown doodad type %d in XkbWriteGeomDoodads\n",
                   doodad->any.type);
            ErrorF("[xkb] Ignored\n");
            break;
        }
    }
    return wire;
}