nfs_lookup_revalidate_negative(struct inode *dir, struct dentry *dentry,
			       unsigned int flags)
{
	int ret = 1;
	if (nfs_neg_need_reval(dir, dentry, flags)) {
		if (flags & LOOKUP_RCU)
			return -ECHILD;
		ret = 0;
	}
	return nfs_lookup_revalidate_done(dir, dentry, NULL, ret);
}