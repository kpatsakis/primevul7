static char *malloc_fullname(const char *base, int baselen, const char *path, int pathlen)
{
	char *fullname = xmalloc(baselen + pathlen + 1);
	memcpy(fullname, base, baselen);
	memcpy(fullname + baselen, path, pathlen);
	fullname[baselen + pathlen] = 0;
	return fullname;
}