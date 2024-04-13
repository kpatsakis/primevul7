xps_new_font(xps_context_t *ctx, byte *buf, int buflen, int index)
{
    xps_font_t *font;
    int code;

    font = xps_alloc(ctx, sizeof(xps_font_t));
    if (!font)
    {
        gs_throw(gs_error_VMerror, "out of memory");
        return NULL;
    }

    font->data = buf;
    font->length = buflen;
    font->font = NULL;

    font->subfontid = index;
    font->cmaptable = 0;
    font->cmapsubcount = 0;
    font->cmapsubtable = 0;
    font->usepua = 0;

    font->cffdata = 0;
    font->cffend = 0;
    font->gsubrs = 0;
    font->subrs = 0;
    font->charstrings = 0;

    if (memcmp(font->data, "OTTO", 4) == 0)
        code = xps_init_postscript_font(ctx, font);
    else if (memcmp(font->data, "\0\1\0\0", 4) == 0)
        code = xps_init_truetype_font(ctx, font);
    else if (memcmp(font->data, "true", 4) == 0)
        code = xps_init_truetype_font(ctx, font);
    else if (memcmp(font->data, "ttcf", 4) == 0)
        code = xps_init_truetype_font(ctx, font);
    else
    {
        xps_free_font(ctx, font);
        gs_throw(-1, "not an opentype font");
        return NULL;
    }

    if (code < 0)
    {
        xps_free_font(ctx, font);
        gs_rethrow(-1, "cannot init font");
        return NULL;
    }

    xps_load_sfnt_cmap(font);

    return font;
}