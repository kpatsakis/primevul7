xps_measure_font_glyph(xps_context_t *ctx, xps_font_t *font, int gid, xps_glyph_metrics_t *mtx)
{

    int head, format, loca, glyf;
    int ofs, len;
    int idx, i, n;
    int hadv, vadv, vorg;
    int vtop, ymax, desc;
    int scale;

    /* some insane defaults */

    scale = 1000; /* units-per-em */
    hadv = 500;
    vadv = -1000;
    vorg = 1000;

    /*
     * Horizontal metrics are easy.
     */

    ofs = xps_find_sfnt_table(font, "hhea", &len);
    if (ofs < 0 || len < 2 * 18)
    {
        gs_warn("hhea table is too short");
        return;
    }

    vorg = s16(font->data + ofs + 4); /* ascender is default vorg */
    desc = s16(font->data + ofs + 6); /* descender */
    if (desc < 0)
        desc = -desc;
    n = u16(font->data + ofs + 17 * 2);

    ofs = xps_find_sfnt_table(font, "hmtx", &len);
    if (ofs < 0)
    {
        gs_warn("cannot find hmtx table");
        return;
    }

    idx = gid;
    if (idx > n - 1)
        idx = n - 1;

    hadv = u16(font->data + ofs + idx * 4);
    vadv = 0;

    /*
     * Vertical metrics are hairy (with missing tables).
     */

    head = xps_find_sfnt_table(font, "head", &len);
    if (head > 0)
    {
        scale = u16(font->data + head + 18); /* units per em */
    }

    ofs = xps_find_sfnt_table(font, "OS/2", &len);
    if (ofs > 0 && len > 70)
    {
        vorg = s16(font->data + ofs + 68); /* sTypoAscender */
        desc = s16(font->data + ofs + 70); /* sTypoDescender */
        if (desc < 0)
            desc = -desc;
    }

    ofs = xps_find_sfnt_table(font, "vhea", &len);
    if (ofs > 0 && len >= 2 * 18)
    {
        n = u16(font->data + ofs + 17 * 2);

        ofs = xps_find_sfnt_table(font, "vmtx", &len);
        if (ofs < 0)
        {
            gs_warn("cannot find vmtx table");
            return;
        }

        idx = gid;
        if (idx > n - 1)
            idx = n - 1;

        vadv = u16(font->data + ofs + idx * 4);
        vtop = u16(font->data + ofs + idx * 4 + 2);

        glyf = xps_find_sfnt_table(font, "glyf", &len);
        loca = xps_find_sfnt_table(font, "loca", &len);
        if (head > 0 && glyf > 0 && loca > 0)
        {
            format = u16(font->data + head + 50); /* indexToLocaFormat */

            if (format == 0)
                ofs = u16(font->data + loca + gid * 2) * 2;
            else
                ofs = u32(font->data + loca + gid * 4);

            ymax = u16(font->data + glyf + ofs + 8); /* yMax */

            vorg = ymax + vtop;
        }
    }

    ofs = xps_find_sfnt_table(font, "VORG", &len);
    if (ofs > 0)
    {
        vorg = u16(font->data + ofs + 6);
        n = u16(font->data + ofs + 6);
        for (i = 0; i < n; i++)
        {
            if (u16(font->data + ofs + 8 + 4 * i) == gid)
            {
                vorg = s16(font->data + ofs + 8 + 4 * i + 2);
                break;
            }
        }
    }

    if (vadv == 0)
        vadv = vorg + desc;

    mtx->hadv = hadv / (float) scale;
    mtx->vadv = vadv / (float) scale;
    mtx->vorg = vorg / (float) scale;
}