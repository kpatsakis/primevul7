irc_color_encode (const char *string, int keep_colors)
{
    unsigned char *out, *ptr_string;
    int out_length, out_pos, length;

    out_length = (strlen (string) * 2) + 1;
    out = malloc (out_length);
    if (!out)
        return NULL;

    ptr_string = (unsigned char *)string;
    out_pos = 0;
    while (ptr_string && ptr_string[0] && (out_pos < out_length - 1))
    {
        switch (ptr_string[0])
        {
            case 0x02: /* ^B */
                if (keep_colors)
                    out[out_pos++] = IRC_COLOR_BOLD_CHAR;
                ptr_string++;
                break;
            case 0x03: /* ^C */
                if (keep_colors)
                    out[out_pos++] = IRC_COLOR_COLOR_CHAR;
                ptr_string++;
                if (isdigit (ptr_string[0]))
                {
                    if (keep_colors)
                        out[out_pos++] = ptr_string[0];
                    ptr_string++;
                    if (isdigit (ptr_string[0]))
                    {
                        if (keep_colors)
                            out[out_pos++] = ptr_string[0];
                        ptr_string++;
                    }
                }
                if (ptr_string[0] == ',')
                {
                    if (keep_colors)
                        out[out_pos++] = ',';
                    ptr_string++;
                    if (isdigit (ptr_string[0]))
                    {
                        if (keep_colors)
                            out[out_pos++] = ptr_string[0];
                        ptr_string++;
                        if (isdigit (ptr_string[0]))
                        {
                            if (keep_colors)
                                out[out_pos++] = ptr_string[0];
                            ptr_string++;
                        }
                    }
                }
                break;
            case 0x0F: /* ^O */
                if (keep_colors)
                    out[out_pos++] = IRC_COLOR_RESET_CHAR;
                ptr_string++;
                break;
            case 0x12: /* ^R */
                if (keep_colors)
                    out[out_pos++] = IRC_COLOR_REVERSE_CHAR;
                ptr_string++;
                break;
            case 0x15: /* ^U */
                if (keep_colors)
                    out[out_pos++] = IRC_COLOR_UNDERLINE_CHAR;
                ptr_string++;
                break;
            default:
                length = weechat_utf8_char_size ((char *)ptr_string);
                if (length == 0)
                    length = 1;
                memcpy (out + out_pos, ptr_string, length);
                out_pos += length;
                ptr_string += length;
        }
    }

    out[out_pos] = '\0';

    return (char *)out;
}