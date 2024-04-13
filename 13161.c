_archive_write_disk_data(struct archive *_a, const void *buff, size_t size)
{
	struct archive_write_disk *a = (struct archive_write_disk *)_a;

	archive_check_magic(&a->archive, ARCHIVE_WRITE_DISK_MAGIC,
	    ARCHIVE_STATE_DATA, "archive_write_data");

	if (a->todo & TODO_HFS_COMPRESSION)
		return (hfs_write_data_block(a, buff, size));
	return (write_data_block(a, buff, size));
}