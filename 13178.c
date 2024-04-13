hfs_reset_compressor(struct archive_write_disk *a)
{
	int ret;

	if (a->stream_valid)
		ret = deflateReset(&a->stream);
	else
		ret = deflateInit(&a->stream, a->decmpfs_compression_level);

	if (ret != Z_OK) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
		    "Failed to initialize compressor");
		return (ARCHIVE_FATAL);
	} else
		a->stream_valid = 1;

	return (ARCHIVE_OK);
}