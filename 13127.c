archive_write_disk_set_user_lookup(struct archive *_a,
    void *private_data,
    int64_t (*lookup_uid)(void *private, const char *uname, int64_t uid),
    void (*cleanup_uid)(void *private))
{
	struct archive_write_disk *a = (struct archive_write_disk *)_a;
	archive_check_magic(&a->archive, ARCHIVE_WRITE_DISK_MAGIC,
	    ARCHIVE_STATE_ANY, "archive_write_disk_set_user_lookup");

	if (a->cleanup_uid != NULL && a->lookup_uid_data != NULL)
		(a->cleanup_uid)(a->lookup_uid_data);

	a->lookup_uid = lookup_uid;
	a->cleanup_uid = cleanup_uid;
	a->lookup_uid_data = private_data;
	return (ARCHIVE_OK);
}