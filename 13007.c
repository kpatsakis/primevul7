file_check_str(gerb_file_t *fd, const char *str)
{
    char c;

    for (int i = 0; str[i] != '\0'; i++) {

	c = gerb_fgetc(fd);

	if (c == EOF)
	    return -1;

	if (c != str[i]) {
	    do {
		/* Restore checked string */
		gerb_ungetc(fd);
	    } while (i--);

	    return 0;
	}
    }

    return 1;
}