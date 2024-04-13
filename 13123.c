archive_write_disk_set_group_lookup(struct archive *_a,
    void *private_data,
    la_int64_t (*lookup_gid)(void *private, const char *gname, la_int64_t gid),
    void (*cleanup_gid)(void *private))
{
	struct archive_write_disk *a = (struct archive_write_disk *)_a;
	archive_check_magic(&a->archive, ARCHIVE_WRITE_DISK_MAGIC,
	    ARCHIVE_STATE_ANY, "archive_write_disk_set_group_lookup");

	if (a->cleanup_gid != NULL && a->lookup_gid_data != NULL)
		(a->cleanup_gid)(a->lookup_gid_data);

	a->lookup_gid = lookup_gid;
	a->cleanup_gid = cleanup_gid;
	a->lookup_gid_data = private_data;
	return (ARCHIVE_OK);
}