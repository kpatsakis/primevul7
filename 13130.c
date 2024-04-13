check_symlinks(struct archive_write_disk *a)
{
	struct archive_string error_string;
	int error_number;
	int rc;
	archive_string_init(&error_string);
	rc = check_symlinks_fsobj(a->name, &error_number, &error_string,
	    a->flags, 0);
	if (rc != ARCHIVE_OK) {
		archive_set_error(&a->archive, error_number, "%s",
		    error_string.s);
	}
	archive_string_free(&error_string);
	a->pst = NULL;	/* to be safe */
	return rc;
}