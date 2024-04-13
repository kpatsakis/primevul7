_archive_write_disk_data_block(struct archive *_a,
    const void *buff, size_t size, int64_t offset)
{
	struct archive_write_disk *a = (struct archive_write_disk *)_a;
	ssize_t r;

	archive_check_magic(&a->archive, ARCHIVE_WRITE_DISK_MAGIC,
	    ARCHIVE_STATE_DATA, "archive_write_data_block");

	a->offset = offset;
	if (a->todo & TODO_HFS_COMPRESSION)
		r = hfs_write_data_block(a, buff, size);
	else
		r = write_data_block(a, buff, size);
	if (r < ARCHIVE_OK)
		return (r);
	if ((size_t)r < size) {
		archive_set_error(&a->archive, 0,
		    "Too much data: Truncating file at %ju bytes",
		    (uintmax_t)a->filesize);
		return (ARCHIVE_WARN);
	}
#if ARCHIVE_VERSION_NUMBER < 3999000
	return (ARCHIVE_OK);
#else
	return (size);
#endif
}