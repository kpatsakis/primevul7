const char *read_gitfile_gently(const char *path)
{
	char *buf;
	struct stat st;
	int fd;
	size_t len;

	if (stat(path, &st))
		return NULL;
	if (!S_ISREG(st.st_mode))
		return NULL;
	fd = open(path, O_RDONLY);
	if (fd < 0)
		die("Error opening %s: %s", path, strerror(errno));
	buf = xmalloc(st.st_size + 1);
	len = read_in_full(fd, buf, st.st_size);
	close(fd);
	if (len != st.st_size)
		die("Error reading %s", path);
	buf[len] = '\0';
	if (prefixcmp(buf, "gitdir: "))
		die("Invalid gitfile format: %s", path);
	while (buf[len - 1] == '\n' || buf[len - 1] == '\r')
		len--;
	if (len < 9)
		die("No path in gitfile: %s", path);
	buf[len] = '\0';
	if (!is_git_directory(buf + 8))
		die("Not a git repository: %s", buf + 8);
	path = make_absolute_path(buf + 8);
	free(buf);
	return path;
}