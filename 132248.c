nfs_lookup_revalidate_done(struct inode *dir, struct dentry *dentry,
			   struct inode *inode, int error)
{
	switch (error) {
	case 1:
		dfprintk(LOOKUPCACHE, "NFS: %s(%pd2) is valid\n",
			__func__, dentry);
		return 1;
	case 0:
		/*
		 * We can't d_drop the root of a disconnected tree:
		 * its d_hash is on the s_anon list and d_drop() would hide
		 * it from shrink_dcache_for_unmount(), leading to busy
		 * inodes on unmount and further oopses.
		 */
		if (inode && IS_ROOT(dentry))
			return 1;
		dfprintk(LOOKUPCACHE, "NFS: %s(%pd2) is invalid\n",
				__func__, dentry);
		return 0;
	}
	dfprintk(LOOKUPCACHE, "NFS: %s(%pd2) lookup returned error %d\n",
				__func__, dentry, error);
	return error;
}