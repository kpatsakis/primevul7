archive_write_disk_set_acls(struct archive *a, int fd, const char *name,
    struct archive_acl *abstract_acl, __LA_MODE_T mode)
{
	(void)a; /* UNUSED */
	(void)fd; /* UNUSED */
	(void)name; /* UNUSED */
	(void)abstract_acl; /* UNUSED */
	(void)mode; /* UNUSED */
	return (ARCHIVE_OK);
}