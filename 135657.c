static void copy_file_with_prefix(FILE *file,
				  int prefix, const char *data, int size,
				  const char *set, const char *reset)
{
	int ch, nl_just_seen = 1;
	while (0 < size--) {
		ch = *data++;
		if (nl_just_seen) {
			fputs(set, file);
			putc(prefix, file);
		}
		if (ch == '\n') {
			nl_just_seen = 1;
			fputs(reset, file);
		} else
			nl_just_seen = 0;
		putc(ch, file);
	}
	if (!nl_just_seen)
		fprintf(file, "%s\n\\ No newline at end of file\n", reset);
}