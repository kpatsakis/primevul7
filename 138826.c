static int ext4_end_io_dio(struct kiocb *iocb, loff_t offset,
			    ssize_t size, void *private)
{
        ext4_io_end_t *io_end = private;

	/* if not async direct IO just return */
	if (!io_end)
		return 0;

	ext_debug("ext4_end_io_dio(): io_end 0x%p "
		  "for inode %lu, iocb 0x%p, offset %llu, size %zd\n",
		  io_end, io_end->inode->i_ino, iocb, offset, size);

	/*
	 * Error during AIO DIO. We cannot convert unwritten extents as the
	 * data was not written. Just clear the unwritten flag and drop io_end.
	 */
	if (size <= 0) {
		ext4_clear_io_unwritten_flag(io_end);
		size = 0;
	}
	io_end->offset = offset;
	io_end->size = size;
	ext4_put_io_end(io_end);

	return 0;
}