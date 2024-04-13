static int selinux_inode_getsecctx(struct inode *inode, void **ctx, u32 *ctxlen)
{
	int len = 0;
	len = selinux_inode_getsecurity(inode, XATTR_SELINUX_SUFFIX,
						ctx, true);
	if (len < 0)
		return len;
	*ctxlen = len;
	return 0;
}