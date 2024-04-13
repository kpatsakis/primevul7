static void read_conf(void)
{
	FILE *fp = fopen(FUSE_CONF, "r");
	if (fp != NULL) {
		int linenum = 1;
		char line[256];
		int isnewline = 1;
		while (fgets(line, sizeof(line), fp) != NULL) {
			if (isnewline) {
				if (line[strlen(line)-1] == '\n') {
					strip_line(line);
					parse_line(line, linenum);
				} else {
					isnewline = 0;
				}
			} else if(line[strlen(line)-1] == '\n') {
				fprintf(stderr, "%s: reading %s: line %i too long\n", progname, FUSE_CONF, linenum);

				isnewline = 1;
			}
			if (isnewline)
				linenum ++;
		}
		if (!isnewline) {
			fprintf(stderr, "%s: reading %s: missing newline at end of file\n", progname, FUSE_CONF);

		}
		fclose(fp);
	} else if (errno != ENOENT) {
		fprintf(stderr, "%s: failed to open %s: %s\n",
			progname, FUSE_CONF, strerror(errno));
	}
}