xps_decode_font_char(xps_font_t *font, int code)
{
    int gid = xps_decode_font_char_imp(font, code);
    if (gid == 0)
        return code;
    return gid;
}