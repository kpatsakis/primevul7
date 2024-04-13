static bool unix_ex_wire_to_permset(connection_struct *conn, unsigned char wire_perm, SMB_ACL_PERMSET_T *p_permset)
{
	if (wire_perm & ~(SMB_POSIX_ACL_READ|SMB_POSIX_ACL_WRITE|SMB_POSIX_ACL_EXECUTE)) {
		return False;
	}

	if (SMB_VFS_SYS_ACL_CLEAR_PERMS(conn, *p_permset) ==  -1) {
		return False;
	}

	if (wire_perm & SMB_POSIX_ACL_READ) {
		if (SMB_VFS_SYS_ACL_ADD_PERM(conn, *p_permset, SMB_ACL_READ) == -1) {
			return False;
		}
	}
	if (wire_perm & SMB_POSIX_ACL_WRITE) {
		if (SMB_VFS_SYS_ACL_ADD_PERM(conn, *p_permset, SMB_ACL_WRITE) == -1) {
			return False;
		}
	}
	if (wire_perm & SMB_POSIX_ACL_EXECUTE) {
		if (SMB_VFS_SYS_ACL_ADD_PERM(conn, *p_permset, SMB_ACL_EXECUTE) == -1) {
			return False;
		}
	}
	return True;
}