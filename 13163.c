_archive_write_disk_free(struct archive *_a)
{
	struct archive_write_disk *a;
	int ret;
	if (_a == NULL)
		return (ARCHIVE_OK);
	archive_check_magic(_a, ARCHIVE_WRITE_DISK_MAGIC,
	    ARCHIVE_STATE_ANY | ARCHIVE_STATE_FATAL, "archive_write_disk_free");
	a = (struct archive_write_disk *)_a;
	ret = _archive_write_disk_close(&a->archive);
	archive_write_disk_set_group_lookup(&a->archive, NULL, NULL, NULL);
	archive_write_disk_set_user_lookup(&a->archive, NULL, NULL, NULL);
	archive_entry_free(a->entry);
	archive_string_free(&a->_name_data);
	archive_string_free(&a->_tmpname_data);
	archive_string_free(&a->archive.error_string);
	archive_string_free(&a->path_safe);
	a->archive.magic = 0;
	__archive_clean(&a->archive);
	free(a->decmpfs_header_p);
	free(a->resource_fork);
	free(a->compressed_buffer);
	free(a->uncompressed_buffer);
#if defined(__APPLE__) && defined(UF_COMPRESSED) && defined(HAVE_SYS_XATTR_H)\
	&& defined(HAVE_ZLIB_H)
	if (a->stream_valid) {
		switch (deflateEnd(&a->stream)) {
		case Z_OK:
			break;
		default:
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "Failed to clean up compressor");
			ret = ARCHIVE_FATAL;
			break;
		}
	}
#endif
	free(a);
	return (ret);
}