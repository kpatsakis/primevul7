la_mktemp(struct archive_write_disk *a)
{
	int oerrno, fd;
	mode_t mode;

	archive_string_empty(&a->_tmpname_data);
	archive_string_sprintf(&a->_tmpname_data, "%s.XXXXXX", a->name);
	a->tmpname = a->_tmpname_data.s;

	fd = __archive_mkstemp(a->tmpname);
	if (fd == -1)
		return -1;

	mode = a->mode & 0777 & ~a->user_umask;
	if (fchmod(fd, mode) == -1) {
		oerrno = errno;
		close(fd);
		errno = oerrno;
		return -1;
	}
	return fd;
}