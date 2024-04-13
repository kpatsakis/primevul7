int get_acl_group_bits( connection_struct *conn, const char *fname, mode_t *mode )
{
	int entry_id = SMB_ACL_FIRST_ENTRY;
	SMB_ACL_ENTRY_T entry;
	SMB_ACL_T posix_acl;
	int result = -1;

	posix_acl = SMB_VFS_SYS_ACL_GET_FILE(conn, fname, SMB_ACL_TYPE_ACCESS);
	if (posix_acl == (SMB_ACL_T)NULL)
		return -1;

	while (SMB_VFS_SYS_ACL_GET_ENTRY(conn, posix_acl, entry_id, &entry) == 1) {
		SMB_ACL_TAG_T tagtype;
		SMB_ACL_PERMSET_T permset;

		/* get_next... */
		if (entry_id == SMB_ACL_FIRST_ENTRY)
			entry_id = SMB_ACL_NEXT_ENTRY;

		if (SMB_VFS_SYS_ACL_GET_TAG_TYPE(conn, entry, &tagtype) ==-1)
			break;

		if (tagtype == SMB_ACL_GROUP_OBJ) {
			if (SMB_VFS_SYS_ACL_GET_PERMSET(conn, entry, &permset) == -1) {
				break;
			} else {
				*mode &= ~(S_IRGRP|S_IWGRP|S_IXGRP);
				*mode |= (SMB_VFS_SYS_ACL_GET_PERM(conn, permset, SMB_ACL_READ) ? S_IRGRP : 0);
				*mode |= (SMB_VFS_SYS_ACL_GET_PERM(conn, permset, SMB_ACL_WRITE) ? S_IWGRP : 0);
				*mode |= (SMB_VFS_SYS_ACL_GET_PERM(conn, permset, SMB_ACL_EXECUTE) ? S_IXGRP : 0);
				result = 0;
				break;
			}
		}
	}
	SMB_VFS_SYS_ACL_FREE_ACL(conn, posix_acl);
	return result;
}