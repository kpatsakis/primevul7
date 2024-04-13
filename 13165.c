hfs_write_resource_fork_header(struct archive_write_disk *a)
{
	unsigned char *buff;
	uint32_t rsrc_bytes;
	uint32_t rsrc_header_bytes;

	/*
	 * Write resource fork header + block info.
	 */
	buff = a->resource_fork;
	rsrc_bytes = a->compressed_rsrc_position - RSRC_F_SIZE;
	rsrc_header_bytes =
		RSRC_H_SIZE +		/* Header base size. */
		4 +			/* Block count. */
		(a->decmpfs_block_count * 8);/* Block info */
	archive_be32enc(buff, 0x100);
	archive_be32enc(buff + 4, rsrc_bytes);
	archive_be32enc(buff + 8, rsrc_bytes - 256);
	archive_be32enc(buff + 12, 0x32);
	memset(buff + 16, 0, 240);
	archive_be32enc(buff + 256, rsrc_bytes - 260);
	return hfs_write_resource_fork(a, buff, rsrc_header_bytes, 0);
}