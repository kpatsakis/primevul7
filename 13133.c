hfs_write_decmpfs(struct archive_write_disk *a)
{
	int r;
	uint32_t compression_type;

	r = fsetxattr(a->fd, DECMPFS_XATTR_NAME, a->decmpfs_header_p,
	    a->decmpfs_attr_size, 0, 0);
	if (r < 0) {
		archive_set_error(&a->archive, errno,
		    "Cannot restore xattr:%s", DECMPFS_XATTR_NAME);
		compression_type = archive_le32dec(
		    &a->decmpfs_header_p[DECMPFS_COMPRESSION_TYPE]);
		if (compression_type == CMP_RESOURCE_FORK)
			fremovexattr(a->fd, XATTR_RESOURCEFORK_NAME,
			    XATTR_SHOWCOMPRESSION);
		return (ARCHIVE_WARN);
	}
	return (ARCHIVE_OK);
}