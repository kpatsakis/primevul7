xps_true_callback_build_char(gs_show_enum *penum, gs_gstate *pgs, gs_font *pfont,
        gs_char chr, gs_glyph glyph)
{
    gs_font_type42 *p42 = (gs_font_type42*)pfont;
    const gs_rect *pbbox;
    float sbw[4], w2[6];
    gs_fixed_point saved_adjust;
    int code;

    // dmprintf1(pfont->memory, "build char ttf %d\n", glyph);

    code = gs_type42_get_metrics(p42, glyph, sbw);
    if (code < 0)
        return code;

    w2[0] = sbw[2];
    w2[1] = sbw[3];

    pbbox =  &p42->FontBBox;
    w2[2] = pbbox->p.x;
    w2[3] = pbbox->p.y;
    w2[4] = pbbox->q.x;
    w2[5] = pbbox->q.y;

    /* Expand the bbox when stroking */
    if ( pfont->PaintType )
    {
        float expand = max(1.415, gs_currentmiterlimit(pgs)) * gs_currentlinewidth(pgs) / 2;
        w2[2] -= expand, w2[3] -= expand;
        w2[4] += expand, w2[5] += expand;
    }

    if ( (code = gs_moveto(pgs, 0.0, 0.0)) < 0 )
        return code;

    if ( (code = gs_setcachedevice(penum, pgs, w2)) < 0 )
        return code;

    code = gs_type42_append(glyph, pgs,
            gx_current_path(pgs),
            (gs_text_enum_t*)penum, (gs_font*)p42,
            gs_show_in_charpath(penum) != cpm_show);
    if (code < 0)
        return code;

    /* Indicate that dropout prevention should be enabled by setting
        fill_adjust to the special value -1. */
    saved_adjust = pgs->fill_adjust;
    pgs->fill_adjust.x = -1;
    pgs->fill_adjust.y = -1;

    code = (pfont->PaintType ? gs_stroke(pgs) : gs_fill(pgs));
    if (code < 0)
        return code;

    pgs->fill_adjust = saved_adjust;

    return 0;
}