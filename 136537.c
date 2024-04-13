irc_color_modifier_cb (void *data, const char *modifier,
                       const char *modifier_data, const char *string)
{
    int keep_colors;

    /* make C compiler happy */
    (void) data;

    keep_colors = (modifier_data && (strcmp (modifier_data, "1") == 0)) ? 1 : 0;

    if (strcmp (modifier, "irc_color_decode") == 0)
        return irc_color_decode (string, keep_colors);

    if (strcmp (modifier, "irc_color_encode") == 0)
        return irc_color_encode (string, keep_colors);

    /* unknown modifier */
    return NULL;
}