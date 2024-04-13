termcap_length(const char *src)
{
    static const char pattern[] = ":\\\n\t:";

    int len = 0;
    const char *const t = src + strlen(src);

    while (*src != '\0') {
	IGNORE_SEP_TRAIL(src, t, pattern);
	src++;
	len++;
    }
    return len;
}