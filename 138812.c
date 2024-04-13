static int ext4_get_block_trans(struct inode *inode, sector_t iblock,
				struct buffer_head *bh_result, int flags)
{
	int dio_credits;
	handle_t *handle;
	int retries = 0;
	int ret;

	/* Trim mapping request to maximum we can map at once for DIO */
	if (bh_result->b_size >> inode->i_blkbits > DIO_MAX_BLOCKS)
		bh_result->b_size = DIO_MAX_BLOCKS << inode->i_blkbits;
	dio_credits = ext4_chunk_trans_blocks(inode,
				      bh_result->b_size >> inode->i_blkbits);
retry:
	handle = ext4_journal_start(inode, EXT4_HT_MAP_BLOCKS, dio_credits);
	if (IS_ERR(handle))
		return PTR_ERR(handle);

	ret = _ext4_get_block(inode, iblock, bh_result, flags);
	ext4_journal_stop(handle);

	if (ret == -ENOSPC && ext4_should_retry_alloc(inode->i_sb, &retries))
		goto retry;
	return ret;
}