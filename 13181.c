set_mac_metadata(struct archive_write_disk *a, const char *pathname,
		 const void *metadata, size_t metadata_size)
{
	(void)a; /* UNUSED */
	(void)pathname; /* UNUSED */
	(void)metadata; /* UNUSED */
	(void)metadata_size; /* UNUSED */
	return (ARCHIVE_OK);
}