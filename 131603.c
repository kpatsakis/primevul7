bool directory_has_default_acl(connection_struct *conn, const char *fname)
{
	SMB_ACL_T def_acl = SMB_VFS_SYS_ACL_GET_FILE( conn, fname, SMB_ACL_TYPE_DEFAULT);
	bool has_acl = False;
	SMB_ACL_ENTRY_T entry;

	if (def_acl != NULL && (SMB_VFS_SYS_ACL_GET_ENTRY(conn, def_acl, SMB_ACL_FIRST_ENTRY, &entry) == 1)) {
		has_acl = True;
	}

	if (def_acl) {
	        SMB_VFS_SYS_ACL_FREE_ACL(conn, def_acl);
	}
        return has_acl;
}