xps_load_sfnt_cmap(xps_font_t *font)
{
    byte *cmapdata;
    int offset, length;
    int nsubtables;

    offset = xps_find_sfnt_table(font, "cmap", &length);
    if (offset < 0 || length < 4)
    {
        gs_warn("cannot find cmap table");
        return;
    }

    cmapdata = font->data + offset;

    nsubtables = u16(cmapdata + 2);
    if (nsubtables < 0 || length < 4 + nsubtables * 8)
    {
        gs_warn("cannot find cmap sub-tables");
        return;
    }

    font->cmaptable = offset;
    font->cmapsubcount = nsubtables;
    font->cmapsubtable = 0;
}