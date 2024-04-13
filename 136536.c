irc_color_decode_for_user_entry (const char *string)
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
            case IRC_COLOR_BOLD_CHAR:
                out[out_pos++] = 0x02;
                ptr_string++;
                break;
            case IRC_COLOR_FIXED_CHAR:
                ptr_string++;
                break;
            case IRC_COLOR_RESET_CHAR:
                out[out_pos++] = 0x0F;
                ptr_string++;
                break;
            case IRC_COLOR_REVERSE_CHAR:
            case IRC_COLOR_REVERSE2_CHAR:
                out[out_pos++] = 0x12;
                ptr_string++;
                break;
            case IRC_COLOR_ITALIC_CHAR:
                out[out_pos++] = 0x1D;
                ptr_string++;
                break;
            case IRC_COLOR_UNDERLINE_CHAR:
                out[out_pos++] = 0x15;
                ptr_string++;
                break;
            case IRC_COLOR_COLOR_CHAR:
                out[out_pos++] = 0x03;
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