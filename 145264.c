help(
	struct parse *pcmd,
	FILE *fp
	)
{
	struct xcmd *xcp = NULL;	/* quiet warning */
	const char *cmd;
	const char *list[100];
	size_t word, words;
	size_t row, rows;
	size_t col, cols;
	size_t length;

	if (pcmd->nargs == 0) {
		words = 0;
		for (xcp = builtins; xcp->keyword != NULL; xcp++) {
			if (*(xcp->keyword) != '?' &&
			    words < COUNTOF(list))
				list[words++] = xcp->keyword;
		}
		for (xcp = opcmds; xcp->keyword != NULL; xcp++)
			if (words < COUNTOF(list))
				list[words++] = xcp->keyword;

		qsort((void *)list, words, sizeof(list[0]), helpsort);
		col = 0;
		for (word = 0; word < words; word++) {
			length = strlen(list[word]);
			col = max(col, length);
		}

		cols = SCREENWIDTH / ++col;
		rows = (words + cols - 1) / cols;

		fprintf(fp, "ntpq commands:\n");

		for (row = 0; row < rows; row++) {
			for (word = row; word < words; word += rows)
				fprintf(fp, "%-*.*s", (int)col,
					(int)col - 1, list[word]);
			fprintf(fp, "\n");
		}
	} else {
		cmd = pcmd->argval[0].string;
		words = findcmd(cmd, builtins, opcmds, &xcp);
		if (words == 0) {
			fprintf(stderr,
				"Command `%s' is unknown\n", cmd);
			return;
		} else if (words >= 2) {
			fprintf(stderr,
				"Command `%s' is ambiguous\n", cmd);
			return;
		}
		fprintf(fp, "function: %s\n", xcp->comment);
		printusage(xcp, fp);
	}
}