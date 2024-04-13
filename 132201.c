nfs_do_lookup_revalidate(struct inode *dir, struct dentry *dentry,
			 unsigned int flags)
{
	struct inode *inode;
	int error;

	nfs_inc_stats(dir, NFSIOS_DENTRYREVALIDATE);
	inode = d_inode(dentry);

	if (!inode)
		return nfs_lookup_revalidate_negative(dir, dentry, flags);

	if (is_bad_inode(inode)) {
		dfprintk(LOOKUPCACHE, "%s: %pd2 has dud inode\n",
				__func__, dentry);
		goto out_bad;
	}

	if (nfs_verifier_is_delegated(dentry))
		return nfs_lookup_revalidate_delegated(dir, dentry, inode);

	/* Force a full look up iff the parent directory has changed */
	if (!(flags & (LOOKUP_EXCL | LOOKUP_REVAL)) &&
	    nfs_check_verifier(dir, dentry, flags & LOOKUP_RCU)) {
		error = nfs_lookup_verify_inode(inode, flags);
		if (error) {
			if (error == -ESTALE)
				nfs_mark_dir_for_revalidate(dir);
			goto out_bad;
		}
		nfs_advise_use_readdirplus(dir);
		goto out_valid;
	}

	if (flags & LOOKUP_RCU)
		return -ECHILD;

	if (NFS_STALE(inode))
		goto out_bad;

	trace_nfs_lookup_revalidate_enter(dir, dentry, flags);
	error = nfs_lookup_revalidate_dentry(dir, dentry, inode);
	trace_nfs_lookup_revalidate_exit(dir, dentry, flags, error);
	return error;
out_valid:
	return nfs_lookup_revalidate_done(dir, dentry, inode, 1);
out_bad:
	if (flags & LOOKUP_RCU)
		return -ECHILD;
	return nfs_lookup_revalidate_done(dir, dentry, inode, 0);
}