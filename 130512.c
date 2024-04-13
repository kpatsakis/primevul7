encode_b64(char *target, char *source, unsigned state, int *saved)
{
    /* RFC-4648 */
    static const char data[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789" "-_";
    int ch = UChar(source[state]);

    switch (state % 3) {
    case 0:
	*target++ = data[(ch >> 2) & 077];
	*saved = (ch << 4);
	break;
    case 1:
	*target++ = data[((ch >> 4) | *saved) & 077];
	*saved = (ch << 2);
	break;
    case 2:
	*target++ = data[((ch >> 6) | *saved) & 077];
	*target++ = data[ch & 077];
	*saved = 0;
	break;
    }
    *target = '\0';
}