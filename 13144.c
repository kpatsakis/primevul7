hfs_write_data_block(struct archive_write_disk *a, const char *buff,
    size_t size)
{
	uint64_t start_size = size;
	ssize_t bytes_written = 0;
	ssize_t bytes_to_write;

	if (size == 0)
		return (ARCHIVE_OK);

	if (a->filesize == 0 || a->fd < 0) {
		archive_set_error(&a->archive, 0,
		    "Attempt to write to an empty file");
		return (ARCHIVE_WARN);
	}

	/* If this write would run beyond the file size, truncate it. */
	if (a->filesize >= 0 && (int64_t)(a->offset + size) > a->filesize)
		start_size = size = (size_t)(a->filesize - a->offset);

	/* Write the data. */
	while (size > 0) {
		bytes_to_write = size;
		/* Seek if necessary to the specified offset. */
		if (a->offset < a->fd_offset) {
			/* Can't support backward move. */
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "Seek failed");
			return (ARCHIVE_FATAL);
		} else if (a->offset > a->fd_offset) {
			int64_t skip = a->offset - a->fd_offset;
			char nullblock[1024];

			memset(nullblock, 0, sizeof(nullblock));
			while (skip > 0) {
				if (skip > (int64_t)sizeof(nullblock))
					bytes_written = hfs_write_decmpfs_block(
					    a, nullblock, sizeof(nullblock));
				else
					bytes_written = hfs_write_decmpfs_block(
					    a, nullblock, skip);
				if (bytes_written < 0) {
					archive_set_error(&a->archive, errno,
					    "Write failed");
					return (ARCHIVE_WARN);
				}
				skip -= bytes_written;
			}

			a->fd_offset = a->offset;
		}
		bytes_written =
		    hfs_write_decmpfs_block(a, buff, bytes_to_write);
		if (bytes_written < 0)
			return (bytes_written);
		buff += bytes_written;
		size -= bytes_written;
		a->total_bytes_written += bytes_written;
		a->offset += bytes_written;
		a->fd_offset = a->offset;
	}
	return (start_size - size);
}