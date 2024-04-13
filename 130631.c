xps_true_callback_decode_glyph(gs_font *pfont, gs_glyph glyph, int ch, ushort *unicode_return, unsigned int length)
{
    xps_font_t *font = pfont->client_data;
    char *ur = (char *)unicode_return;
    int u;

    if (length == 0)
        return 2;
    u = xps_decode_font_char(font, glyph);
    /* Unfortunate assumptions in the pdfwrite code mea that we have to return the
     * value as a big-endian short, no matter what platform we are on
     */
    ur[1] = u & 0xff;
    ur[0] = u >> 8;
    return 2;
}