int fchmod_acl(files_struct *fsp, mode_t mode)
{
	connection_struct *conn = fsp->conn;
	SMB_ACL_T posix_acl = NULL;
	int ret = -1;

	if ((posix_acl = SMB_VFS_SYS_ACL_GET_FD(fsp)) == NULL)
		return -1;

	if ((ret = chmod_acl_internals(conn, posix_acl, mode)) == -1)
		goto done;

	ret = SMB_VFS_SYS_ACL_SET_FD(fsp, posix_acl);

  done:

	SMB_VFS_SYS_ACL_FREE_ACL(conn, posix_acl);
	return ret;
}