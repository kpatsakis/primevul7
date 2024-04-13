hfs_write_decmpfs_block(struct archive_write_disk *a, const char *buff,
    size_t size)
{
	const char *buffer_to_write;
	size_t bytes_to_write;
	int ret;

	if (a->decmpfs_block_count == (unsigned)-1) {
		void *new_block;
		size_t new_size;
		unsigned int block_count;

		if (a->decmpfs_header_p == NULL) {
			new_block = malloc(MAX_DECMPFS_XATTR_SIZE
			    + sizeof(uint32_t));
			if (new_block == NULL) {
				archive_set_error(&a->archive, ENOMEM,
				    "Can't allocate memory for decmpfs");
				return (ARCHIVE_FATAL);
			}
			a->decmpfs_header_p = new_block;
		}
		a->decmpfs_attr_size = DECMPFS_HEADER_SIZE;
		archive_le32enc(&a->decmpfs_header_p[DECMPFS_COMPRESSION_MAGIC],
		    DECMPFS_MAGIC);
		archive_le32enc(&a->decmpfs_header_p[DECMPFS_COMPRESSION_TYPE],
		    CMP_RESOURCE_FORK);
		archive_le64enc(&a->decmpfs_header_p[DECMPFS_UNCOMPRESSED_SIZE],
		    a->filesize);

		/* Calculate a block count of the file. */
		block_count =
		    (a->filesize + MAX_DECMPFS_BLOCK_SIZE -1) /
			MAX_DECMPFS_BLOCK_SIZE;
		/*
		 * Allocate buffer for resource fork.
		 * Set up related pointers;
		 */
		new_size =
		    RSRC_H_SIZE + /* header */
		    4 + /* Block count */
		    (block_count * sizeof(uint32_t) * 2) +
		    RSRC_F_SIZE; /* footer */
		if (new_size > a->resource_fork_allocated_size) {
			new_block = realloc(a->resource_fork, new_size);
			if (new_block == NULL) {
				archive_set_error(&a->archive, ENOMEM,
				    "Can't allocate memory for ResourceFork");
				return (ARCHIVE_FATAL);
			}
			a->resource_fork_allocated_size = new_size;
			a->resource_fork = new_block;
		}

		/* Allocate uncompressed buffer */
		if (a->uncompressed_buffer == NULL) {
			new_block = malloc(MAX_DECMPFS_BLOCK_SIZE);
			if (new_block == NULL) {
				archive_set_error(&a->archive, ENOMEM,
				    "Can't allocate memory for decmpfs");
				return (ARCHIVE_FATAL);
			}
			a->uncompressed_buffer = new_block;
		}
		a->block_remaining_bytes = MAX_DECMPFS_BLOCK_SIZE;
		a->file_remaining_bytes = a->filesize;
		a->compressed_buffer_remaining = a->compressed_buffer_size;

		/*
		 * Set up a resource fork.
		 */
		a->rsrc_xattr_options = XATTR_CREATE;
		/* Get the position where we are going to set a bunch
		 * of block info. */
		a->decmpfs_block_info =
		    (uint32_t *)(a->resource_fork + RSRC_H_SIZE);
		/* Set the block count to the resource fork. */
		archive_le32enc(a->decmpfs_block_info++, block_count);
		/* Get the position where we are going to set compressed
		 * data. */
		a->compressed_rsrc_position =
		    RSRC_H_SIZE + 4 + (block_count * 8);
		a->compressed_rsrc_position_v = a->compressed_rsrc_position;
		a->decmpfs_block_count = block_count;
	}

	/* Ignore redundant bytes. */
	if (a->file_remaining_bytes == 0)
		return ((ssize_t)size);

	/* Do not overrun a block size. */
	if (size > a->block_remaining_bytes)
		bytes_to_write = a->block_remaining_bytes;
	else
		bytes_to_write = size;
	/* Do not overrun the file size. */
	if (bytes_to_write > a->file_remaining_bytes)
		bytes_to_write = a->file_remaining_bytes;

	/* For efficiency, if a copy length is full of the uncompressed
	 * buffer size, do not copy writing data to it. */
	if (bytes_to_write == MAX_DECMPFS_BLOCK_SIZE)
		buffer_to_write = buff;
	else {
		memcpy(a->uncompressed_buffer +
		    MAX_DECMPFS_BLOCK_SIZE - a->block_remaining_bytes,
		    buff, bytes_to_write);
		buffer_to_write = a->uncompressed_buffer;
	}
	a->block_remaining_bytes -= bytes_to_write;
	a->file_remaining_bytes -= bytes_to_write;

	if (a->block_remaining_bytes == 0 || a->file_remaining_bytes == 0) {
		ret = hfs_drive_compressor(a, buffer_to_write,
		    MAX_DECMPFS_BLOCK_SIZE - a->block_remaining_bytes);
		if (ret < 0)
			return (ret);
		a->block_remaining_bytes = MAX_DECMPFS_BLOCK_SIZE;
	}
	/* Ignore redundant bytes. */
	if (a->file_remaining_bytes == 0)
		return ((ssize_t)size);
	return (bytes_to_write);
}