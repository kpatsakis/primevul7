NTSTATUS posix_get_nt_acl(struct connection_struct *conn, const char *name,
			  uint32_t security_info, SEC_DESC **ppdesc)
{
	SMB_STRUCT_STAT sbuf;
	SMB_ACL_T posix_acl = NULL;
	SMB_ACL_T def_acl = NULL;
	struct pai_val *pal;

	*ppdesc = NULL;

	DEBUG(10,("posix_get_nt_acl: called for file %s\n", name ));

	/* Get the stat struct for the owner info. */
	if(SMB_VFS_STAT(conn, name, &sbuf) != 0) {
		return map_nt_error_from_unix(errno);
	}

	/* Get the ACL from the path. */
	posix_acl = SMB_VFS_SYS_ACL_GET_FILE(conn, name, SMB_ACL_TYPE_ACCESS);

	/* If it's a directory get the default POSIX ACL. */
	if(S_ISDIR(sbuf.st_mode)) {
		def_acl = SMB_VFS_SYS_ACL_GET_FILE(conn, name, SMB_ACL_TYPE_DEFAULT);
		def_acl = free_empty_sys_acl(conn, def_acl);
	}

	pal = load_inherited_info(conn, name);

	return posix_get_nt_acl_common(conn, name, &sbuf, pal, posix_acl,
				       def_acl, security_info, ppdesc);
}