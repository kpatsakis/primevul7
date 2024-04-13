static int ext4_dio_get_block_overwrite(struct inode *inode, sector_t iblock,
		   struct buffer_head *bh_result, int create)
{
	int ret;

	ext4_debug("ext4_dio_get_block_overwrite: inode %lu, create flag %d\n",
		   inode->i_ino, create);
	/* We don't expect handle for direct IO */
	WARN_ON_ONCE(ext4_journal_current_handle());

	ret = _ext4_get_block(inode, iblock, bh_result, 0);
	/*
	 * Blocks should have been preallocated! ext4_file_write_iter() checks
	 * that.
	 */
	WARN_ON_ONCE(!buffer_mapped(bh_result) || buffer_unwritten(bh_result));

	return ret;
}