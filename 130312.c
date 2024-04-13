int jas_stream_display(jas_stream_t *stream, FILE *fp, int n)
{
	unsigned char buf[16];
	int i;
	int j;
	int m;
	int c;
	int display;
	int cnt;

	cnt = n - (n % 16);
	display = 1;

	for (i = 0; i < n; i += 16) {
		if (n > 16 && i > 0) {
			display = (i >= cnt) ? 1 : 0;
		}
		if (display) {
			fprintf(fp, "%08x:", i);
		}
		m = JAS_MIN(n - i, 16);
		for (j = 0; j < m; ++j) {
			if ((c = jas_stream_getc(stream)) == EOF) {
				abort();
				return -1;
			}
			buf[j] = c;
		}
		if (display) {
			for (j = 0; j < m; ++j) {
				fprintf(fp, " %02x", buf[j]);
			}
			fputc(' ', fp);
			for (; j < 16; ++j) {
				fprintf(fp, "   ");
			}
			for (j = 0; j < m; ++j) {
				if (isprint(buf[j])) {
					fputc(buf[j], fp);
				} else {
					fputc(' ', fp);
				}
			}
			fprintf(fp, "\n");
		}


	}
	return 0;
}