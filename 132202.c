nfs4_do_lookup_revalidate(struct inode *dir, struct dentry *dentry,
			  unsigned int flags)
{
	struct inode *inode;

	if (!(flags & LOOKUP_OPEN) || (flags & LOOKUP_DIRECTORY))
		goto full_reval;
	if (d_mountpoint(dentry))
		goto full_reval;

	inode = d_inode(dentry);

	/* We can't create new files in nfs_open_revalidate(), so we
	 * optimize away revalidation of negative dentries.
	 */
	if (inode == NULL)
		goto full_reval;

	if (nfs_verifier_is_delegated(dentry))
		return nfs_lookup_revalidate_delegated(dir, dentry, inode);

	/* NFS only supports OPEN on regular files */
	if (!S_ISREG(inode->i_mode))
		goto full_reval;

	/* We cannot do exclusive creation on a positive dentry */
	if (flags & (LOOKUP_EXCL | LOOKUP_REVAL))
		goto reval_dentry;

	/* Check if the directory changed */
	if (!nfs_check_verifier(dir, dentry, flags & LOOKUP_RCU))
		goto reval_dentry;

	/* Let f_op->open() actually open (and revalidate) the file */
	return 1;
reval_dentry:
	if (flags & LOOKUP_RCU)
		return -ECHILD;
	return nfs_lookup_revalidate_dentry(dir, dentry, inode);

full_reval:
	return nfs_do_lookup_revalidate(dir, dentry, flags);
}