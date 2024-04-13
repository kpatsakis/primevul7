static int ext4_dio_get_block_unwritten_sync(struct inode *inode,
		sector_t iblock, struct buffer_head *bh_result,	int create)
{
	int ret;

	/* We don't expect handle for direct IO */
	WARN_ON_ONCE(ext4_journal_current_handle());

	ret = ext4_get_block_trans(inode, iblock, bh_result,
				   EXT4_GET_BLOCKS_IO_CREATE_EXT);

	/*
	 * Mark inode as having pending DIO writes to unwritten extents.
	 * ext4_direct_IO_write() checks this flag and converts extents to
	 * written.
	 */
	if (!ret && buffer_unwritten(bh_result))
		ext4_set_inode_state(inode, EXT4_STATE_DIO_UNWRITTEN);

	return ret;
}