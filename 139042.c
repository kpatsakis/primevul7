log_open_file (char *servname, char *channame, char *netname)
{
	char buf[512];
	int fd;
	char *file;
	time_t currenttime;

	file = log_create_pathname (servname, channame, netname);
	if (!file)
		return -1;

	fd = g_open (file, O_CREAT | O_APPEND | O_WRONLY | OFLAGS, 0644);
	g_free (file);

	if (fd == -1)
		return -1;
	currenttime = time (NULL);
	write (fd, buf,
			 g_snprintf (buf, sizeof (buf), _("**** BEGIN LOGGING AT %s\n"),
						  ctime (&currenttime)));

	return fd;
}