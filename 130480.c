show_entry(void)
{
    /*
     * Trim any remaining whitespace.
     */
    if (outbuf.used != 0) {
	bool infodump = !TcOutput();
	char delim = (char) (infodump ? ',' : ':');
	int j;

	for (j = (int) outbuf.used - 1; j > 0; --j) {
	    char ch = outbuf.text[j];
	    if (ch == '\n') {
		;
	    } else if (isspace(UChar(ch))) {
		outbuf.used = (size_t) j;
	    } else if (!infodump && ch == '\\') {
		outbuf.used = (size_t) j;
	    } else if (ch == delim && (j == 0 || outbuf.text[j - 1] != '\\')) {
		outbuf.used = (size_t) (j + 1);
	    } else {
		break;
	    }
	}
	outbuf.text[outbuf.used] = '\0';
    }
    if (outbuf.text != 0) {
	(void) fputs(outbuf.text, stdout);
	putchar('\n');
    }
    return (int) outbuf.used;
}