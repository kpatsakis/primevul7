copy_xattrs(struct archive_write_disk *a, int tmpfd, int dffd)
{
	ssize_t xattr_size;
	char *xattr_names = NULL, *xattr_val = NULL;
	int ret = ARCHIVE_OK, xattr_i;

	xattr_size = flistxattr(tmpfd, NULL, 0, 0);
	if (xattr_size == -1) {
		archive_set_error(&a->archive, errno,
		    "Failed to read metadata(xattr)");
		ret = ARCHIVE_WARN;
		goto exit_xattr;
	}
	xattr_names = malloc(xattr_size);
	if (xattr_names == NULL) {
		archive_set_error(&a->archive, ENOMEM,
		    "Can't allocate memory for metadata(xattr)");
		ret = ARCHIVE_FATAL;
		goto exit_xattr;
	}
	xattr_size = flistxattr(tmpfd, xattr_names, xattr_size, 0);
	if (xattr_size == -1) {
		archive_set_error(&a->archive, errno,
		    "Failed to read metadata(xattr)");
		ret = ARCHIVE_WARN;
		goto exit_xattr;
	}
	for (xattr_i = 0; xattr_i < xattr_size;
	    xattr_i += strlen(xattr_names + xattr_i) + 1) {
		char *xattr_val_saved;
		ssize_t s;
		int f;

		s = fgetxattr(tmpfd, xattr_names + xattr_i, NULL, 0, 0, 0);
		if (s == -1) {
			archive_set_error(&a->archive, errno,
			    "Failed to get metadata(xattr)");
			ret = ARCHIVE_WARN;
			goto exit_xattr;
		}
		xattr_val_saved = xattr_val;
		xattr_val = realloc(xattr_val, s);
		if (xattr_val == NULL) {
			archive_set_error(&a->archive, ENOMEM,
			    "Failed to get metadata(xattr)");
			ret = ARCHIVE_WARN;
			free(xattr_val_saved);
			goto exit_xattr;
		}
		s = fgetxattr(tmpfd, xattr_names + xattr_i, xattr_val, s, 0, 0);
		if (s == -1) {
			archive_set_error(&a->archive, errno,
			    "Failed to get metadata(xattr)");
			ret = ARCHIVE_WARN;
			goto exit_xattr;
		}
		f = fsetxattr(dffd, xattr_names + xattr_i, xattr_val, s, 0, 0);
		if (f == -1) {
			archive_set_error(&a->archive, errno,
			    "Failed to get metadata(xattr)");
			ret = ARCHIVE_WARN;
			goto exit_xattr;
		}
	}
exit_xattr:
	free(xattr_names);
	free(xattr_val);
	return (ret);
}