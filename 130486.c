strncpy_DYN(DYNBUF * dst, const char *src, size_t need)
{
    size_t want = need + dst->used + 1;
    if (want > dst->size) {
	dst->size += (want + 1024);	/* be generous */
	dst->text = typeRealloc(char, dst->size, dst->text);
	if (dst->text == 0)
	    failed("strncpy_DYN");
    }
    _nc_STRNCPY(dst->text + dst->used, src, need + 1);
    dst->used += need;
    dst->text[dst->used] = 0;
}