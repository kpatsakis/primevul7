output(
	FILE *fp,
	const char *name,
	const char *value
	)
{
	size_t len;

	/* strlen of "name=value" */
	len = strlen(name) + 1 + strlen(value);

	if (out_chars != 0) {
		out_chars += 2;
		if ((out_linecount + len + 2) > MAXOUTLINE) {
			fputs(",\n", fp);
			out_linecount = 0;
		} else {
			fputs(", ", fp);
			out_linecount += 2;
		}
	}

	fputs(name, fp);
	putc('=', fp);
	fputs(value, fp);
	out_chars += len;
	out_linecount += len;
}