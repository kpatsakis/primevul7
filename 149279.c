varexpand(struct magic_set *ms, char *buf, size_t len, const char *str)
{
	const char *ptr, *sptr, *e, *t, *ee, *et;
	size_t l;

	for (sptr = str; (ptr = strstr(sptr, "${")) != NULL;) {
		l = (size_t)(ptr - sptr);
		if (l >= len)
			return -1;
		memcpy(buf, sptr, l);
		buf += l;
		len -= l;
		ptr += 2;
		if (!*ptr || ptr[1] != '?')
			return -1;
		for (et = t = ptr + 2; *et && *et != ':'; et++)
			continue;
		if (*et != ':')
			return -1;
		for (ee = e = et + 1; *ee && *ee != '}'; ee++)
			continue;
		if (*ee != '}')
			return -1;
		switch (*ptr) {
		case 'x':
			if (ms->mode & 0111) {
				ptr = t;
				l = et - t;
			} else {
				ptr = e;
				l = ee - e;
			}
			break;
		default:
			return -1;
		}
		if (l >= len)
			return -1;
		memcpy(buf, ptr, l);
		buf += l;
		len -= l;
		sptr = ee + 1;
	}

	l = strlen(sptr);
	if (l >= len)
		return -1;

	memcpy(buf, sptr, l);
	buf[l] = '\0';
	return 0;
}