int ext4_dio_get_block(struct inode *inode, sector_t iblock,
		       struct buffer_head *bh, int create)
{
	/* We don't expect handle for direct IO */
	WARN_ON_ONCE(ext4_journal_current_handle());

	if (!create)
		return _ext4_get_block(inode, iblock, bh, 0);
	return ext4_get_block_trans(inode, iblock, bh, EXT4_GET_BLOCKS_CREATE);
}