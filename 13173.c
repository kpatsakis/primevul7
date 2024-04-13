hfs_set_compressed_fflag(struct archive_write_disk *a)
{
	int r;

	if ((r = lazy_stat(a)) != ARCHIVE_OK)
		return (r);

	a->st.st_flags |= UF_COMPRESSED;
	if (fchflags(a->fd, a->st.st_flags) != 0) {
		archive_set_error(&a->archive, errno,
		    "Failed to set UF_COMPRESSED file flag");
		return (ARCHIVE_WARN);
	}
	return (ARCHIVE_OK);
}