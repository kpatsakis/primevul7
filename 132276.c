__nfs_lookup_revalidate(struct dentry *dentry, unsigned int flags,
			int (*reval)(struct inode *, struct dentry *, unsigned int))
{
	struct dentry *parent;
	struct inode *dir;
	int ret;

	if (flags & LOOKUP_RCU) {
		parent = READ_ONCE(dentry->d_parent);
		dir = d_inode_rcu(parent);
		if (!dir)
			return -ECHILD;
		ret = reval(dir, dentry, flags);
		if (parent != READ_ONCE(dentry->d_parent))
			return -ECHILD;
	} else {
		parent = dget_parent(dentry);
		ret = reval(d_inode(parent), dentry, flags);
		dput(parent);
	}
	return ret;
}