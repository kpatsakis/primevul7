xps_encode_font_char(xps_font_t *font, int code)
{
    int gid = xps_encode_font_char_imp(font, code);
    if (gid == 0 && font->usepua)
        gid = xps_encode_font_char_imp(font, 0xF000 | code);
    return gid;
}