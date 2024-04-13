static int copy_access_acl(connection_struct *conn, const char *from, const char *to, mode_t mode)
{
	SMB_ACL_T posix_acl = NULL;
	int ret = -1;

	if ((posix_acl = SMB_VFS_SYS_ACL_GET_FILE(conn, from, SMB_ACL_TYPE_ACCESS)) == NULL)
		return -1;

	if ((ret = chmod_acl_internals(conn, posix_acl, mode)) == -1)
		goto done;

	ret = SMB_VFS_SYS_ACL_SET_FILE(conn, to, SMB_ACL_TYPE_ACCESS, posix_acl);

 done:

	SMB_VFS_SYS_ACL_FREE_ACL(conn, posix_acl);
	return ret;
}