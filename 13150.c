create_tempdatafork(struct archive_write_disk *a, const char *pathname)
{
	struct archive_string tmpdatafork;
	int tmpfd;

	archive_string_init(&tmpdatafork);
	archive_strcpy(&tmpdatafork, "tar.md.XXXXXX");
	tmpfd = mkstemp(tmpdatafork.s);
	if (tmpfd < 0) {
		archive_set_error(&a->archive, errno,
		    "Failed to mkstemp");
		archive_string_free(&tmpdatafork);
		return (-1);
	}
	if (copyfile(pathname, tmpdatafork.s, 0,
	    COPYFILE_UNPACK | COPYFILE_NOFOLLOW
	    | COPYFILE_ACL | COPYFILE_XATTR) < 0) {
		archive_set_error(&a->archive, errno,
		    "Failed to restore metadata");
		close(tmpfd);
		tmpfd = -1;
	}
	unlink(tmpdatafork.s);
	archive_string_free(&tmpdatafork);
	return (tmpfd);
}