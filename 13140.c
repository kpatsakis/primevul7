hfs_write_compressed_data(struct archive_write_disk *a, size_t bytes_compressed)
{
	int ret;

	ret = hfs_write_resource_fork(a, a->compressed_buffer,
	    bytes_compressed, a->compressed_rsrc_position);
	if (ret == ARCHIVE_OK)
		a->compressed_rsrc_position += bytes_compressed;
	return (ret);
}