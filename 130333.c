int jas_stream_puts(jas_stream_t *stream, const char *s)
{
	while (*s != '\0') {
		if (jas_stream_putc_macro(stream, *s) == EOF) {
			return -1;
		}
		++s;
	}
	return 0;
}