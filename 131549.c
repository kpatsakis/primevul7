static mode_t convert_permset_to_mode_t(connection_struct *conn, SMB_ACL_PERMSET_T permset)
{
	mode_t ret = 0;

	ret |= (SMB_VFS_SYS_ACL_GET_PERM(conn, permset, SMB_ACL_READ) ? S_IRUSR : 0);
	ret |= (SMB_VFS_SYS_ACL_GET_PERM(conn, permset, SMB_ACL_WRITE) ? S_IWUSR : 0);
	ret |= (SMB_VFS_SYS_ACL_GET_PERM(conn, permset, SMB_ACL_EXECUTE) ? S_IXUSR : 0);

	return ret;
}