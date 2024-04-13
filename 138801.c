static ssize_t ext4_direct_IO(struct kiocb *iocb, struct iov_iter *iter)
{
	struct file *file = iocb->ki_filp;
	struct inode *inode = file->f_mapping->host;
	size_t count = iov_iter_count(iter);
	loff_t offset = iocb->ki_pos;
	ssize_t ret;

#ifdef CONFIG_EXT4_FS_ENCRYPTION
	if (ext4_encrypted_inode(inode) && S_ISREG(inode->i_mode))
		return 0;
#endif

	/*
	 * If we are doing data journalling we don't support O_DIRECT
	 */
	if (ext4_should_journal_data(inode))
		return 0;

	/* Let buffer I/O handle the inline data case. */
	if (ext4_has_inline_data(inode))
		return 0;

	/* DAX uses iomap path now */
	if (WARN_ON_ONCE(IS_DAX(inode)))
		return 0;

	trace_ext4_direct_IO_enter(inode, offset, count, iov_iter_rw(iter));
	if (iov_iter_rw(iter) == READ)
		ret = ext4_direct_IO_read(iocb, iter);
	else
		ret = ext4_direct_IO_write(iocb, iter);
	trace_ext4_direct_IO_exit(inode, offset, count, iov_iter_rw(iter), ret);
	return ret;
}