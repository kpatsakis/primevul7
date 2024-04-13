gadgetfs_fill_super (struct super_block *sb, struct fs_context *fc)
{
	struct inode	*inode;
	struct dev_data	*dev;

	if (the_device)
		return -ESRCH;

	CHIP = usb_get_gadget_udc_name();
	if (!CHIP)
		return -ENODEV;

	/* superblock */
	sb->s_blocksize = PAGE_SIZE;
	sb->s_blocksize_bits = PAGE_SHIFT;
	sb->s_magic = GADGETFS_MAGIC;
	sb->s_op = &gadget_fs_operations;
	sb->s_time_gran = 1;

	/* root inode */
	inode = gadgetfs_make_inode (sb,
			NULL, &simple_dir_operations,
			S_IFDIR | S_IRUGO | S_IXUGO);
	if (!inode)
		goto Enomem;
	inode->i_op = &simple_dir_inode_operations;
	if (!(sb->s_root = d_make_root (inode)))
		goto Enomem;

	/* the ep0 file is named after the controller we expect;
	 * user mode code can use it for sanity checks, like we do.
	 */
	dev = dev_new ();
	if (!dev)
		goto Enomem;

	dev->sb = sb;
	dev->dentry = gadgetfs_create_file(sb, CHIP, dev, &ep0_operations);
	if (!dev->dentry) {
		put_dev(dev);
		goto Enomem;
	}

	/* other endpoint files are available after hardware setup,
	 * from binding to a controller.
	 */
	the_device = dev;
	return 0;

Enomem:
	kfree(CHIP);
	CHIP = NULL;

	return -ENOMEM;
}