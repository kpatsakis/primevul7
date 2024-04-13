xps_true_callback_string_proc(gs_font_type42 *p42, ulong offset, uint length, const byte **pdata)
{
    xps_font_t *font = p42->client_data;
    if (offset + length > font->length)
    {
        *pdata = NULL;
        return gs_throw2(-1, "font data access out of bounds (offset=%lu size=%u)", offset, length);
    }
    *pdata = font->data + offset;
    return 0;
}