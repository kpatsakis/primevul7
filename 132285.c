nfs_add_or_obtain(struct dentry *dentry, struct nfs_fh *fhandle,
				struct nfs_fattr *fattr)
{
	struct dentry *parent = dget_parent(dentry);
	struct inode *dir = d_inode(parent);
	struct inode *inode;
	struct dentry *d;
	int error;

	d_drop(dentry);

	if (fhandle->size == 0) {
		error = NFS_PROTO(dir)->lookup(dir, dentry, fhandle, fattr);
		if (error)
			goto out_error;
	}
	nfs_set_verifier(dentry, nfs_save_change_attribute(dir));
	if (!(fattr->valid & NFS_ATTR_FATTR)) {
		struct nfs_server *server = NFS_SB(dentry->d_sb);
		error = server->nfs_client->rpc_ops->getattr(server, fhandle,
				fattr, NULL);
		if (error < 0)
			goto out_error;
	}
	inode = nfs_fhget(dentry->d_sb, fhandle, fattr);
	d = d_splice_alias(inode, dentry);
out:
	dput(parent);
	return d;
out_error:
	d = ERR_PTR(error);
	goto out;
}