static int selinux_inode_alloc_security(struct inode *inode)
{
	return inode_alloc_security(inode);
}