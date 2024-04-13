static long long read_xattr_ids()
{
	int res;
	struct squashfs_xattr_table id_table;

	if(sBlk.s.xattr_id_table_start == SQUASHFS_INVALID_BLK)
		return 0;

	/*
	 * Read xattr id table, containing start of xattr metadata and the
	 * number of xattrs in the file system
	 */
	res = read_fs_bytes(fd, sBlk.s.xattr_id_table_start, sizeof(id_table),
		&id_table);
	if(res == FALSE)
		return -1;

	SQUASHFS_INSWAP_XATTR_TABLE(&id_table);

	return id_table.xattr_ids;
}