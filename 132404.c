bool is_shared_mountpoint(const char *path)
{
	char buf[LXC_LINELEN];
	FILE *f;
	int i;
	char *p, *p2;

	f = fopen("/proc/self/mountinfo", "r");
	if (!f)
		return 0;

	while (fgets(buf, LXC_LINELEN, f)) {
		for (p = buf, i = 0; p && i < 4; i++)
			p = strchr(p + 1, ' ');
		if (!p)
			continue;

		p2 = strchr(p + 1, ' ');
		if (!p2)
			continue;

		*p2 = '\0';
		if (strcmp(p + 1, path) == 0) {
			/* This is the path. Is it shared? */
			p = strchr(p2 + 1, ' ');
			if (p && strstr(p, "shared:")) {
				fclose(f);
				return true;
			}
		}
	}

	fclose(f);
	return false;
}