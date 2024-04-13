static int ext4_iomap_end(struct inode *inode, loff_t offset, loff_t length,
			  ssize_t written, unsigned flags, struct iomap *iomap)
{
	int ret = 0;
	handle_t *handle;
	int blkbits = inode->i_blkbits;
	bool truncate = false;

	if (!(flags & IOMAP_WRITE) || (flags & IOMAP_FAULT))
		return 0;

	handle = ext4_journal_start(inode, EXT4_HT_INODE, 2);
	if (IS_ERR(handle)) {
		ret = PTR_ERR(handle);
		goto orphan_del;
	}
	if (ext4_update_inode_size(inode, offset + written))
		ext4_mark_inode_dirty(handle, inode);
	/*
	 * We may need to truncate allocated but not written blocks beyond EOF.
	 */
	if (iomap->offset + iomap->length > 
	    ALIGN(inode->i_size, 1 << blkbits)) {
		ext4_lblk_t written_blk, end_blk;

		written_blk = (offset + written) >> blkbits;
		end_blk = (offset + length) >> blkbits;
		if (written_blk < end_blk && ext4_can_truncate(inode))
			truncate = true;
	}
	/*
	 * Remove inode from orphan list if we were extending a inode and
	 * everything went fine.
	 */
	if (!truncate && inode->i_nlink &&
	    !list_empty(&EXT4_I(inode)->i_orphan))
		ext4_orphan_del(handle, inode);
	ext4_journal_stop(handle);
	if (truncate) {
		ext4_truncate_failed_write(inode);
orphan_del:
		/*
		 * If truncate failed early the inode might still be on the
		 * orphan list; we need to make sure the inode is removed from
		 * the orphan list in that case.
		 */
		if (inode->i_nlink)
			ext4_orphan_del(NULL, inode);
	}
	return ret;
}