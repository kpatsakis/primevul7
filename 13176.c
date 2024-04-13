hfs_write_resource_fork(struct archive_write_disk *a, unsigned char *buff,
    size_t bytes, uint32_t position)
{
	int ret;

	ret = fsetxattr(a->fd, XATTR_RESOURCEFORK_NAME, buff, bytes,
	    position, a->rsrc_xattr_options);
	if (ret < 0) {
		archive_set_error(&a->archive, errno,
		    "Cannot restore xattr: %s at %u pos %u bytes",
		    XATTR_RESOURCEFORK_NAME,
		    (unsigned)position,
		    (unsigned)bytes);
		return (ARCHIVE_WARN);
	}
	a->rsrc_xattr_options &= ~XATTR_CREATE;
	return (ARCHIVE_OK);
}