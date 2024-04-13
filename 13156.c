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
		int e;

		archive_entry_xattr_next(entry, &name, &value, &size);

		if (name == NULL)
			continue;
#if ARCHIVE_XATTR_LINUX
		/* Linux: quietly skip POSIX.1e ACL extended attributes */
		if (strncmp(name, "system.", 7) == 0 &&
		   (strcmp(name + 7, "posix_acl_access") == 0 ||
		    strcmp(name + 7, "posix_acl_default") == 0))
			continue;
		if (strncmp(name, "trusted.SGI_", 12) == 0 &&
		   (strcmp(name + 12, "ACL_DEFAULT") == 0 ||
		    strcmp(name + 12, "ACL_FILE") == 0))
			continue;

		/* Linux: xfsroot namespace is obsolete and unsupported */
		if (strncmp(name, "xfsroot.", 8) == 0) {
			fail = 1;
			archive_strcat(&errlist, name);
			archive_strappend_char(&errlist, ' ');
			continue;
		}
#endif

		if (a->fd >= 0) {
#if ARCHIVE_XATTR_LINUX
			e = fsetxattr(a->fd, name, value, size, 0);
#elif ARCHIVE_XATTR_DARWIN
			e = fsetxattr(a->fd, name, value, size, 0, 0);
#elif ARCHIVE_XATTR_AIX
			e = fsetea(a->fd, name, value, size, 0);
#endif
		} else {
#if ARCHIVE_XATTR_LINUX
			e = lsetxattr(archive_entry_pathname(entry),
			    name, value, size, 0);
#elif ARCHIVE_XATTR_DARWIN
			e = setxattr(archive_entry_pathname(entry),
			    name, value, size, 0, XATTR_NOFOLLOW);
#elif ARCHIVE_XATTR_AIX
			e = lsetea(archive_entry_pathname(entry),
			    name, value, size, 0);
#endif
		}
		if (e == -1) {
			ret = ARCHIVE_WARN;
			archive_strcat(&errlist, name);
			archive_strappend_char(&errlist, ' ');
			if (errno != ENOTSUP && errno != ENOSYS)
				fail = 1;
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