strcpy_DYN(DYNBUF * dst, const char *src)
{
    if (src == 0) {
	dst->used = 0;
	strcpy_DYN(dst, "");
    } else {
	strncpy_DYN(dst, src, strlen(src));
    }
}