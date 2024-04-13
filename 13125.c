archive_write_disk_uid(struct archive *_a, const char *name, la_int64_t id)
{
	struct archive_write_disk *a = (struct archive_write_disk *)_a;
	archive_check_magic(&a->archive, ARCHIVE_WRITE_DISK_MAGIC,
	    ARCHIVE_STATE_ANY, "archive_write_disk_uid");
	if (a->lookup_uid)
		return (a->lookup_uid)(a->lookup_uid_data, name, id);
	return (id);
}