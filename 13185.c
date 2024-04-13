hfs_write_data_block(struct archive_write_disk *a, const char *buff,
    size_t size)
{
	return (write_data_block(a, buff, size));
}