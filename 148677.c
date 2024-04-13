gx_ttfReader__default_get_metrics(const ttfReader *ttf, uint glyph_index, bool bVertical,
                                  short *sideBearing, unsigned short *nAdvance)
{
    gx_ttfReader *self = (gx_ttfReader *)ttf;
    float sbw[4];
    int sbw_offset = bVertical;
    int code;
    int factor = self->pfont->data.unitsPerEm;

    code = self->pfont->data.get_metrics(self->pfont, glyph_index, bVertical, sbw);
    if (code < 0)
        return code;
    /* Due to an obsolete convention, simple_glyph_metrics scales
       the metrics into 1x1 rectangle as Postscript like.
       In same time, the True Type interpreter needs
       the original design units.
       Undo the scaling here with accurate rounding. */
    *sideBearing = (short)floor(sbw[0 + sbw_offset] * factor + 0.5);
    *nAdvance = (short)floor(sbw[2 + sbw_offset] * factor + 0.5);
    return 0;
}