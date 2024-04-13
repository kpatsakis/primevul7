static ssize_t ext4_direct_IO_read(struct kiocb *iocb, struct iov_iter *iter)
{
	struct address_space *mapping = iocb->ki_filp->f_mapping;
	struct inode *inode = mapping->host;
	size_t count = iov_iter_count(iter);
	ssize_t ret;

	/*
	 * Shared inode_lock is enough for us - it protects against concurrent
	 * writes & truncates and since we take care of writing back page cache,
	 * we are protected against page writeback as well.
	 */
	inode_lock_shared(inode);
	ret = filemap_write_and_wait_range(mapping, iocb->ki_pos,
					   iocb->ki_pos + count - 1);
	if (ret)
		goto out_unlock;
	ret = __blockdev_direct_IO(iocb, inode, inode->i_sb->s_bdev,
				   iter, ext4_dio_get_block, NULL, NULL, 0);
out_unlock:
	inode_unlock_shared(inode);
	return ret;
}