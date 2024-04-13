trim_trailing(void)
{
    while (outbuf.used > 0 && outbuf.text[outbuf.used - 1] == ' ')
	outbuf.text[--outbuf.used] = '\0';
}