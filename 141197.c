static int selinux_inode_notifysecctx(struct inode *inode, void *ctx, u32 ctxlen)
{
	return selinux_inode_setsecurity(inode, XATTR_SELINUX_SUFFIX, ctx, ctxlen, 0);
}