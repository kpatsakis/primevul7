textfieldrep(Str s, int width)
{
    Lineprop c_type;
    Str n = Strnew_size(width + 2);
    int i, j, k, c_len;

    j = 0;
    for (i = 0; i < s->length; i += c_len) {
	c_type = get_mctype((unsigned char *)&s->ptr[i]);
	c_len = get_mclen(&s->ptr[i]);
	if (s->ptr[i] == '\r')
	    continue;
	k = j + get_mcwidth(&s->ptr[i]);
	if (k > width)
	    break;
	if (c_type == PC_CTRL)
	    Strcat_char(n, ' ');
#ifdef USE_M17N
	else if (c_type & PC_UNKNOWN)
	    Strcat_char(n, ' ');
#endif
	else if (s->ptr[i] == '&')
	    Strcat_charp(n, "&amp;");
	else if (s->ptr[i] == '<')
	    Strcat_charp(n, "&lt;");
	else if (s->ptr[i] == '>')
	    Strcat_charp(n, "&gt;");
	else
	    Strcat_charp_n(n, &s->ptr[i], c_len);
	j = k;
    }
    for (; j < width; j++)
	Strcat_char(n, ' ');
    return n;
}