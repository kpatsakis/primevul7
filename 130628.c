xps_identify_font_encoding(xps_font_t *font, int idx, int *pid, int *eid)
{
    byte *cmapdata, *entry;
    if (idx < 0 || idx >= font->cmapsubcount)
        return;
    cmapdata = font->data + font->cmaptable;
    entry = cmapdata + 4 + idx * 8;
    *pid = u16(entry + 0);
    *eid = u16(entry + 2);
}