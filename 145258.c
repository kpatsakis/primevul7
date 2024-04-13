dump_hex_printable(
	const void *	data,
	size_t		len
	)
{
	const char *	cdata;
	const char *	rowstart;
	size_t		idx;
	size_t		rowlen;
	u_char		uch;

	cdata = data;
	while (len > 0) {
		rowstart = cdata;
		rowlen = min(16, len);
		for (idx = 0; idx < rowlen; idx++) {
			uch = *(cdata++);
			printf("%02x ", uch);
		}
		for ( ; idx < 16 ; idx++)
			printf("   ");
		cdata = rowstart;
		for (idx = 0; idx < rowlen; idx++) {
			uch = *(cdata++);
			printf("%c", (isprint(uch))
					 ? uch
					 : '.');
		}
		printf("\n");
		len -= rowlen;
	}
}