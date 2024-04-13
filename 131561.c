static int map_acl_perms_to_permset(connection_struct *conn, mode_t mode, SMB_ACL_PERMSET_T *p_permset)
{
	if (SMB_VFS_SYS_ACL_CLEAR_PERMS(conn, *p_permset) ==  -1)
		return -1;
	if (mode & S_IRUSR) {
		if (SMB_VFS_SYS_ACL_ADD_PERM(conn, *p_permset, SMB_ACL_READ) == -1)
			return -1;
	}
	if (mode & S_IWUSR) {
		if (SMB_VFS_SYS_ACL_ADD_PERM(conn, *p_permset, SMB_ACL_WRITE) == -1)
			return -1;
	}
	if (mode & S_IXUSR) {
		if (SMB_VFS_SYS_ACL_ADD_PERM(conn, *p_permset, SMB_ACL_EXECUTE) == -1)
			return -1;
	}
	return 0;
}