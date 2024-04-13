getcmds(void)
{
	char *	line;
	int	count;

	ntp_readline_init(interactive ? prompt : NULL);

	for (;;) {
		line = ntp_readline(&count);
		if (NULL == line)
			break;
		docmd(line);
		free(line);
	}

	ntp_readline_uninit();
}