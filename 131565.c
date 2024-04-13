static void store_inheritance_attributes(files_struct *fsp, canon_ace *file_ace_list,
					canon_ace *dir_ace_list, bool pai_protected)
{
	int ret;
	size_t store_size;
	char *pai_buf;

	if (!lp_map_acl_inherit(SNUM(fsp->conn)))
		return;

	/*
	 * Don't store if this ACL isn't protected and
	 * none of the entries in it are marked as inherited.
	 */

	if (!pai_protected && num_inherited_entries(file_ace_list) == 0 && num_inherited_entries(dir_ace_list) == 0) {
		/* Instead just remove the attribute if it exists. */
		if (fsp->fh->fd != -1)
			SMB_VFS_FREMOVEXATTR(fsp, SAMBA_POSIX_INHERITANCE_EA_NAME);
		else
			SMB_VFS_REMOVEXATTR(fsp->conn, fsp->fsp_name, SAMBA_POSIX_INHERITANCE_EA_NAME);
		return;
	}

	pai_buf = create_pai_buf(file_ace_list, dir_ace_list, pai_protected, &store_size);

	if (fsp->fh->fd != -1)
		ret = SMB_VFS_FSETXATTR(fsp, SAMBA_POSIX_INHERITANCE_EA_NAME,
				pai_buf, store_size, 0);
	else
		ret = SMB_VFS_SETXATTR(fsp->conn,fsp->fsp_name, SAMBA_POSIX_INHERITANCE_EA_NAME,
				pai_buf, store_size, 0);

	SAFE_FREE(pai_buf);

	DEBUG(10,("store_inheritance_attribute:%s for file %s\n", pai_protected ? " (protected)" : "", fsp->fsp_name));
	if (ret == -1 && !no_acl_syscall_error(errno))
		DEBUG(1,("store_inheritance_attribute: Error %s\n", strerror(errno) ));
}