nfs_closedir(struct inode *inode, struct file *filp)
{
	put_nfs_open_dir_context(file_inode(filp), filp->private_data);
	return 0;
}