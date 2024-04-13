nfs_lookup_revalidate_delegated(struct inode *dir, struct dentry *dentry,
				struct inode *inode)
{
	nfs_set_verifier(dentry, nfs_save_change_attribute(dir));
	return nfs_lookup_revalidate_done(dir, dentry, inode, 1);
}