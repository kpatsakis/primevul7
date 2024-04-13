gif_read_error(Gif_Context *gfc, int is_error, const char *text)
{
    Gif_ReadErrorHandler handler = gfc->handler ? gfc->handler : default_error_handler;
    if (is_error >= 0)
        gfc->errors[is_error > 0] += 1;
    if (handler)
        handler(gfc->stream, gfc->gfi, is_error, text);
}