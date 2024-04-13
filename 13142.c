cleanup_pathname(struct archive_write_disk *a)
{
	struct archive_string error_string;
	int error_number;
	int rc;
	archive_string_init(&error_string);
	rc = cleanup_pathname_fsobj(a->name, &error_number, &error_string,
	    a->flags);
	if (rc != ARCHIVE_OK) {
		archive_set_error(&a->archive, error_number, "%s",
		    error_string.s);
	}
	archive_string_free(&error_string);
	return rc;
}