fixup_appledouble(struct archive_write_disk *a, const char *pathname)
{
	char buff[8];
	struct stat st;
	const char *p;
	struct archive_string datafork;
	int fd = -1, ret = ARCHIVE_OK;

	archive_string_init(&datafork);
	/* Check if the current file name is a type of the resource
	 * fork file. */
	p = strrchr(pathname, '/');
	if (p == NULL)
		p = pathname;
	else
		p++;
	if (p[0] != '.' || p[1] != '_')
		goto skip_appledouble;

	/*
	 * Check if the data fork file exists.
	 *
	 * TODO: Check if this write disk object has handled it.
	 */
	archive_strncpy(&datafork, pathname, p - pathname);
	archive_strcat(&datafork, p + 2);
	if (lstat(datafork.s, &st) == -1 ||
	    (st.st_mode & AE_IFMT) != AE_IFREG)
		goto skip_appledouble;

	/*
	 * Check if the file is in the AppleDouble form.
	 */
	fd = open(pathname, O_RDONLY | O_BINARY | O_CLOEXEC);
	__archive_ensure_cloexec_flag(fd);
	if (fd == -1) {
		archive_set_error(&a->archive, errno,
		    "Failed to open a restoring file");
		ret = ARCHIVE_WARN;
		goto skip_appledouble;
	}
	if (read(fd, buff, 8) == -1) {
		archive_set_error(&a->archive, errno,
		    "Failed to read a restoring file");
		close(fd);
		ret = ARCHIVE_WARN;
		goto skip_appledouble;
	}
	close(fd);
	/* Check AppleDouble Magic Code. */
	if (archive_be32dec(buff) != 0x00051607)
		goto skip_appledouble;
	/* Check AppleDouble Version. */
	if (archive_be32dec(buff+4) != 0x00020000)
		goto skip_appledouble;

	ret = copy_metadata(a, pathname, datafork.s,
#if defined(UF_COMPRESSED)
	    st.st_flags & UF_COMPRESSED);
#else
	    0);
#endif
	if (ret == ARCHIVE_OK) {
		unlink(pathname);
		ret = ARCHIVE_EOF;
	}
skip_appledouble:
	archive_string_free(&datafork);
	return (ret);
}