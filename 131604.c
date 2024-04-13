bool set_unix_posix_default_acl(connection_struct *conn, const char *fname, SMB_STRUCT_STAT *psbuf,
				uint16 num_def_acls, const char *pdata)
{
	SMB_ACL_T def_acl = NULL;

	if (!S_ISDIR(psbuf->st_mode)) {
		if (num_def_acls) {
			DEBUG(5,("set_unix_posix_default_acl: Can't set default ACL on non-directory file %s\n", fname ));
			errno = EISDIR;
			return False;
		} else {
			return True;
		}
	}

	if (!num_def_acls) {
		/* Remove the default ACL. */
		if (SMB_VFS_SYS_ACL_DELETE_DEF_FILE(conn, fname) == -1) {
			DEBUG(5,("set_unix_posix_default_acl: acl_delete_def_file failed on directory %s (%s)\n",
				fname, strerror(errno) ));
			return False;
		}
		return True;
	}

	if ((def_acl = create_posix_acl_from_wire(conn, num_def_acls, pdata)) == NULL) {
		return False;
	}

	if (SMB_VFS_SYS_ACL_SET_FILE(conn, fname, SMB_ACL_TYPE_DEFAULT, def_acl) == -1) {
		DEBUG(5,("set_unix_posix_default_acl: acl_set_file failed on directory %s (%s)\n",
			fname, strerror(errno) ));
	        SMB_VFS_SYS_ACL_FREE_ACL(conn, def_acl);
		return False;
	}

	DEBUG(10,("set_unix_posix_default_acl: set default acl for file %s\n", fname ));
	SMB_VFS_SYS_ACL_FREE_ACL(conn, def_acl);
	return True;
}