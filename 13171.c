set_xattrs(struct archive_write_disk *a)
{
	struct archive_entry *entry = a->entry;
	struct archive_string errlist;
	int ret = ARCHIVE_OK;
	int i = archive_entry_xattr_reset(entry);
	short fail = 0;

	archive_string_init(&errlist);

	while (i--) {
		const char *name;
		const void *value;
		size_t size;
		archive_entry_xattr_next(entry, &name, &value, &size);
		if (name != NULL) {
			int e;
			int namespace;

			namespace = EXTATTR_NAMESPACE_USER;

			if (strncmp(name, "user.", 5) == 0) {
				/* "user." attributes go to user namespace */
				name += 5;
				namespace = EXTATTR_NAMESPACE_USER;
			} else if (strncmp(name, "system.", 7) == 0) {
				name += 7;
				namespace = EXTATTR_NAMESPACE_SYSTEM;
				if (!strcmp(name, "nfs4.acl") ||
				    !strcmp(name, "posix1e.acl_access") ||
				    !strcmp(name, "posix1e.acl_default"))
					continue;
			} else {
				/* Other namespaces are unsupported */
				archive_strcat(&errlist, name);
				archive_strappend_char(&errlist, ' ');
				fail = 1;
				ret = ARCHIVE_WARN;
				continue;
			}

			if (a->fd >= 0) {
				/*
				 * On FreeBSD, extattr_set_fd does not
				 * return the same as
				 * extattr_set_file. It returns zero
				 * on success, non-zero on failure.
				 *
				 * We can detect the failure by
				 * manually setting errno prior to the
				 * call and checking after.
				 *
				 * If errno remains zero, fake the
				 * return value by setting e to size.
				 *
				 * This is a hack for now until I
				 * (Shawn Webb) get FreeBSD to fix the
				 * issue, if that's even possible.
				 */
				errno = 0;
				e = extattr_set_fd(a->fd, namespace, name,
				    value, size);
				if (e == 0 && errno == 0) {
					e = size;
				}
			} else {
				e = extattr_set_link(
				    archive_entry_pathname(entry), namespace,
				    name, value, size);
			}
			if (e != (int)size) {
				archive_strcat(&errlist, name);
				archive_strappend_char(&errlist, ' ');
				ret = ARCHIVE_WARN;
				if (errno != ENOTSUP && errno != ENOSYS)
					fail = 1;
			}
		}
	}

	if (ret == ARCHIVE_WARN) {
		if (fail && errlist.length > 0) {
			errlist.length--;
			errlist.s[errlist.length] = '\0';

			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "Cannot restore extended attributes: %s",
			    errlist.s);
		} else
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "Cannot restore extended "
			    "attributes on this file system.");
	}

	archive_string_free(&errlist);
	return (ret);
}