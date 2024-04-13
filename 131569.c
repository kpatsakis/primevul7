SMB_ACL_T free_empty_sys_acl(connection_struct *conn, SMB_ACL_T the_acl)
{
	SMB_ACL_ENTRY_T entry;

	if (!the_acl)
		return NULL;
	if (SMB_VFS_SYS_ACL_GET_ENTRY(conn, the_acl, SMB_ACL_FIRST_ENTRY, &entry) != 1) {
		SMB_VFS_SYS_ACL_FREE_ACL(conn, the_acl);
		return NULL;
	}
	return the_acl;
}