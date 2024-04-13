nfs_lookup_revalidate_dentry(struct inode *dir, struct dentry *dentry,
			     struct inode *inode)
{
	struct nfs_fh *fhandle;
	struct nfs_fattr *fattr;
	unsigned long dir_verifier;
	int ret;

	ret = -ENOMEM;
	fhandle = nfs_alloc_fhandle();
	fattr = nfs_alloc_fattr_with_label(NFS_SERVER(inode));
	if (fhandle == NULL || fattr == NULL)
		goto out;

	dir_verifier = nfs_save_change_attribute(dir);
	ret = NFS_PROTO(dir)->lookup(dir, dentry, fhandle, fattr);
	if (ret < 0) {
		switch (ret) {
		case -ESTALE:
		case -ENOENT:
			ret = 0;
			break;
		case -ETIMEDOUT:
			if (NFS_SERVER(inode)->flags & NFS_MOUNT_SOFTREVAL)
				ret = 1;
		}
		goto out;
	}
	ret = 0;
	if (nfs_compare_fh(NFS_FH(inode), fhandle))
		goto out;
	if (nfs_refresh_inode(inode, fattr) < 0)
		goto out;

	nfs_setsecurity(inode, fattr);
	nfs_set_verifier(dentry, dir_verifier);

	/* set a readdirplus hint that we had a cache miss */
	nfs_force_use_readdirplus(dir);
	ret = 1;
out:
	nfs_free_fattr(fattr);
	nfs_free_fhandle(fhandle);

	/*
	 * If the lookup failed despite the dentry change attribute being
	 * a match, then we should revalidate the directory cache.
	 */
	if (!ret && nfs_dentry_verify_change(dir, dentry))
		nfs_mark_dir_for_revalidate(dir);
	return nfs_lookup_revalidate_done(dir, dentry, inode, ret);
}