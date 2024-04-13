xps_find_sfnt_table(xps_font_t *font, const char *name, int *lengthp)
{
    int offset;
    int ntables;
    int i;

    if (font->length < 12)
        return -1;

    if (!memcmp(font->data, "ttcf", 4))
    {
        int nfonts = u32(font->data + 8);
        if (font->subfontid < 0 || font->subfontid >= nfonts)
        {
            gs_warn("Invalid subfont ID");
            return -1;
        }
        offset = u32(font->data + 12 + font->subfontid * 4);
    }
    else
    {
        offset = 0;
    }

    ntables = u16(font->data + offset + 4);
    if (font->length < offset + 12 + ntables * 16)
        return -1;

    for (i = 0; i < ntables; i++)
    {
        byte *entry = font->data + offset + 12 + i * 16;
        if (!memcmp(entry, name, 4))
        {
            if (lengthp)
                *lengthp = u32(entry + 12);
            return u32(entry + 8);
        }
    }

    return -1;
}