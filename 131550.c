bool set_unix_posix_acl(connection_struct *conn, files_struct *fsp, const char *fname, uint16 num_acls, const char *pdata)
{
	SMB_ACL_T file_acl = NULL;

	if (!num_acls) {
		/* Remove the ACL from the file. */
		return remove_posix_acl(conn, fsp, fname);
	}

	if ((file_acl = create_posix_acl_from_wire(conn, num_acls, pdata)) == NULL) {
		return False;
	}

	if (fsp && fsp->fh->fd != -1) {
		/* The preferred way - use an open fd. */
		if (SMB_VFS_SYS_ACL_SET_FD(fsp, file_acl) == -1) {
			DEBUG(5,("set_unix_posix_acl: acl_set_file failed on %s (%s)\n",
				fname, strerror(errno) ));
		        SMB_VFS_SYS_ACL_FREE_ACL(conn, file_acl);
			return False;
		}
	} else {
		if (SMB_VFS_SYS_ACL_SET_FILE(conn, fname, SMB_ACL_TYPE_ACCESS, file_acl) == -1) {
			DEBUG(5,("set_unix_posix_acl: acl_set_file failed on %s (%s)\n",
				fname, strerror(errno) ));
		        SMB_VFS_SYS_ACL_FREE_ACL(conn, file_acl);
			return False;
		}
	}

	DEBUG(10,("set_unix_posix_acl: set acl for file %s\n", fname ));
	SMB_VFS_SYS_ACL_FREE_ACL(conn, file_acl);
	return True;
}