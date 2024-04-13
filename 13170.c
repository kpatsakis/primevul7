hfs_decompress(struct archive_write_disk *a)
{
	uint32_t *block_info;
	unsigned int block_count;
	uint32_t data_pos, data_size;
	ssize_t r;
	ssize_t bytes_written, bytes_to_write;
	unsigned char *b;

	block_info = (uint32_t *)(a->resource_fork + RSRC_H_SIZE);
	block_count = archive_le32dec(block_info++);
	while (block_count--) {
		data_pos = RSRC_H_SIZE + archive_le32dec(block_info++);
		data_size = archive_le32dec(block_info++);
		r = fgetxattr(a->fd, XATTR_RESOURCEFORK_NAME,
		    a->compressed_buffer, data_size, data_pos, 0);
		if (r != data_size)  {
			archive_set_error(&a->archive,
			    (r < 0)?errno:ARCHIVE_ERRNO_MISC,
			    "Failed to read resource fork");
			return (ARCHIVE_WARN);
		}
		if (a->compressed_buffer[0] == 0xff) {
			bytes_to_write = data_size -1;
			b = a->compressed_buffer + 1;
		} else {
			uLong dest_len = MAX_DECMPFS_BLOCK_SIZE;
			int zr;

			zr = uncompress((Bytef *)a->uncompressed_buffer,
			    &dest_len, a->compressed_buffer, data_size);
			if (zr != Z_OK) {
				archive_set_error(&a->archive,
				    ARCHIVE_ERRNO_MISC,
				    "Failed to decompress resource fork");
				return (ARCHIVE_WARN);
			}
			bytes_to_write = dest_len;
			b = (unsigned char *)a->uncompressed_buffer;
		}
		do {
			bytes_written = write(a->fd, b, bytes_to_write);
			if (bytes_written < 0) {
				archive_set_error(&a->archive, errno,
				    "Write failed");
				return (ARCHIVE_WARN);
			}
			bytes_to_write -= bytes_written;
			b += bytes_written;
		} while (bytes_to_write > 0);
	}
	r = fremovexattr(a->fd, XATTR_RESOURCEFORK_NAME, 0);
	if (r == -1)  {
		archive_set_error(&a->archive, errno,
		    "Failed to remove resource fork");
		return (ARCHIVE_WARN);
	}
	return (ARCHIVE_OK);
}