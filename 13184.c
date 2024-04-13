hfs_drive_compressor(struct archive_write_disk *a, const char *buff,
    size_t size)
{
	unsigned char *buffer_compressed;
	size_t bytes_compressed;
	size_t bytes_used;
	int ret;

	ret = hfs_reset_compressor(a);
	if (ret != ARCHIVE_OK)
		return (ret);

	if (a->compressed_buffer == NULL) {
		size_t block_size;

		block_size = COMPRESSED_W_SIZE + RSRC_F_SIZE +
		    + compressBound(MAX_DECMPFS_BLOCK_SIZE);
		a->compressed_buffer = malloc(block_size);
		if (a->compressed_buffer == NULL) {
			archive_set_error(&a->archive, ENOMEM,
			    "Can't allocate memory for Resource Fork");
			return (ARCHIVE_FATAL);
		}
		a->compressed_buffer_size = block_size;
		a->compressed_buffer_remaining = block_size;
	}

	buffer_compressed = a->compressed_buffer +
	    a->compressed_buffer_size - a->compressed_buffer_remaining;
	a->stream.next_in = (Bytef *)(uintptr_t)(const void *)buff;
	a->stream.avail_in = size;
	a->stream.next_out = buffer_compressed;
	a->stream.avail_out = a->compressed_buffer_remaining;
	do {
		ret = deflate(&a->stream, Z_FINISH);
		switch (ret) {
		case Z_OK:
		case Z_STREAM_END:
			break;
		default:
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "Failed to compress data");
			return (ARCHIVE_FAILED);
		}
	} while (ret == Z_OK);
	bytes_compressed = a->compressed_buffer_remaining - a->stream.avail_out;

	/*
	 * If the compressed size is larger than the original size,
	 * throw away compressed data, use uncompressed data instead.
	 */
	if (bytes_compressed > size) {
		buffer_compressed[0] = 0xFF;/* uncompressed marker. */
		memcpy(buffer_compressed + 1, buff, size);
		bytes_compressed = size + 1;
	}
	a->compressed_buffer_remaining -= bytes_compressed;

	/*
	 * If the compressed size is smaller than MAX_DECMPFS_XATTR_SIZE
	 * and the block count in the file is only one, store compressed
	 * data to decmpfs xattr instead of the resource fork.
	 */
	if (a->decmpfs_block_count == 1 &&
	    (a->decmpfs_attr_size + bytes_compressed)
	      <= MAX_DECMPFS_XATTR_SIZE) {
		archive_le32enc(&a->decmpfs_header_p[DECMPFS_COMPRESSION_TYPE],
		    CMP_XATTR);
		memcpy(a->decmpfs_header_p + DECMPFS_HEADER_SIZE,
		    buffer_compressed, bytes_compressed);
		a->decmpfs_attr_size += bytes_compressed;
		a->compressed_buffer_remaining = a->compressed_buffer_size;
		/*
		 * Finish HFS+ Compression.
		 * - Write the decmpfs xattr.
		 * - Set the UF_COMPRESSED file flag.
		 */
		ret = hfs_write_decmpfs(a);
		if (ret == ARCHIVE_OK)
			ret = hfs_set_compressed_fflag(a);
		return (ret);
	}

	/* Update block info. */
	archive_le32enc(a->decmpfs_block_info++,
	    a->compressed_rsrc_position_v - RSRC_H_SIZE);
	archive_le32enc(a->decmpfs_block_info++, bytes_compressed);
	a->compressed_rsrc_position_v += bytes_compressed;

	/*
	 * Write the compressed data to the resource fork.
	 */
	bytes_used = a->compressed_buffer_size - a->compressed_buffer_remaining;
	while (bytes_used >= COMPRESSED_W_SIZE) {
		ret = hfs_write_compressed_data(a, COMPRESSED_W_SIZE);
		if (ret != ARCHIVE_OK)
			return (ret);
		bytes_used -= COMPRESSED_W_SIZE;
		if (bytes_used > COMPRESSED_W_SIZE)
			memmove(a->compressed_buffer,
			    a->compressed_buffer + COMPRESSED_W_SIZE,
			    bytes_used);
		else
			memcpy(a->compressed_buffer,
			    a->compressed_buffer + COMPRESSED_W_SIZE,
			    bytes_used);
	}
	a->compressed_buffer_remaining = a->compressed_buffer_size - bytes_used;

	/*
	 * If the current block is the last block, write the remaining
	 * compressed data and the resource fork footer.
	 */
	if (a->file_remaining_bytes == 0) {
		size_t rsrc_size;
		int64_t bk;

		/* Append the resource footer. */
		rsrc_size = hfs_set_resource_fork_footer(
		    a->compressed_buffer + bytes_used,
		    a->compressed_buffer_remaining);
		ret = hfs_write_compressed_data(a, bytes_used + rsrc_size);
		a->compressed_buffer_remaining = a->compressed_buffer_size;

		/* If the compressed size is not enough smaller than
		 * the uncompressed size. cancel HFS+ compression.
		 * TODO: study a behavior of ditto utility and improve
		 * the condition to fall back into no HFS+ compression. */
		bk = HFS_BLOCKS(a->compressed_rsrc_position);
		bk += bk >> 7;
		if (bk > HFS_BLOCKS(a->filesize))
			return hfs_decompress(a);
		/*
		 * Write the resourcefork header.
		 */
		if (ret == ARCHIVE_OK)
			ret = hfs_write_resource_fork_header(a);
		/*
		 * Finish HFS+ Compression.
		 * - Write the decmpfs xattr.
		 * - Set the UF_COMPRESSED file flag.
		 */
		if (ret == ARCHIVE_OK)
			ret = hfs_write_decmpfs(a);
		if (ret == ARCHIVE_OK)
			ret = hfs_set_compressed_fflag(a);
	}
	return (ret);
}