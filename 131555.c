NTSTATUS posix_fget_nt_acl(struct files_struct *fsp, uint32_t security_info,
			   SEC_DESC **ppdesc)
{
	SMB_STRUCT_STAT sbuf;
	SMB_ACL_T posix_acl = NULL;
	struct pai_val *pal;

	*ppdesc = NULL;

	DEBUG(10,("posix_fget_nt_acl: called for file %s\n", fsp->fsp_name ));

	/* can it happen that fsp_name == NULL ? */
	if (fsp->is_directory ||  fsp->fh->fd == -1) {
		return posix_get_nt_acl(fsp->conn, fsp->fsp_name,
					security_info, ppdesc);
	}

	/* Get the stat struct for the owner info. */
	if(SMB_VFS_FSTAT(fsp, &sbuf) != 0) {
		return map_nt_error_from_unix(errno);
	}

	/* Get the ACL from the fd. */
	posix_acl = SMB_VFS_SYS_ACL_GET_FD(fsp);

	pal = fload_inherited_info(fsp);

	return posix_get_nt_acl_common(fsp->conn, fsp->fsp_name, &sbuf, pal,
				       posix_acl, NULL, security_info, ppdesc);
}