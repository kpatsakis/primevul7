static void call_nt_transact_query_security_desc(connection_struct *conn,
						 struct smb_request *req,
						 uint16 **ppsetup,
						 uint32 setup_count,
						 char **ppparams,
						 uint32 parameter_count,
						 char **ppdata,
						 uint32 data_count,
						 uint32 max_data_count)
{
	char *params = *ppparams;
	char *data = *ppdata;
	SEC_DESC *psd = NULL;
	size_t sd_size;
	uint32 security_info_wanted;
	files_struct *fsp = NULL;
	NTSTATUS status;
	DATA_BLOB blob;

        if(parameter_count < 8) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		return;
	}

	fsp = file_fsp(req, SVAL(params,0));
	if(!fsp) {
		reply_nterror(req, NT_STATUS_INVALID_HANDLE);
		return;
	}

	security_info_wanted = IVAL(params,4);

	DEBUG(3,("call_nt_transact_query_security_desc: file = %s, "
		 "info_wanted = 0x%x\n", fsp_str_dbg(fsp),
		 (unsigned int)security_info_wanted));

	params = nttrans_realloc(ppparams, 4);
	if(params == NULL) {
		reply_nterror(req, NT_STATUS_NO_MEMORY);
		return;
	}

	/*
	 * Get the permissions to return.
	 */

	if ((security_info_wanted & SECINFO_SACL) &&
			!(fsp->access_mask & SEC_FLAG_SYSTEM_SECURITY)) {
		reply_nterror(req,  NT_STATUS_ACCESS_DENIED);
		return;
	}

	if ((security_info_wanted & (SECINFO_DACL|SECINFO_OWNER|SECINFO_GROUP)) &&
			!(fsp->access_mask & SEC_STD_READ_CONTROL)) {
		reply_nterror(req, NT_STATUS_ACCESS_DENIED);
		return;
	}

	if (security_info_wanted & (SECINFO_DACL|SECINFO_OWNER|
			SECINFO_GROUP|SECINFO_SACL)) {
		/* Don't return SECINFO_LABEL if anything else was
		   requested. See bug #8458. */
		security_info_wanted &= ~SECINFO_LABEL;
	}

	if (!lp_nt_acl_support(SNUM(conn))) {
		status = get_null_nt_acl(talloc_tos(), &psd);
	} else if (security_info_wanted & SECINFO_LABEL) {
		/* Like W2K3 return a null object. */
		status = get_null_nt_acl(talloc_tos(), &psd);
	} else {
		status = SMB_VFS_FGET_NT_ACL(
			fsp, security_info_wanted, &psd);
	}
	if (!NT_STATUS_IS_OK(status)) {
		reply_nterror(req, status);
		return;
	}

	if (!(security_info_wanted & SECINFO_OWNER)) {
		psd->owner_sid = NULL;
	}
	if (!(security_info_wanted & SECINFO_GROUP)) {
		psd->group_sid = NULL;
	}
	if (!(security_info_wanted & SECINFO_DACL)) {
		psd->type &= ~SEC_DESC_DACL_PRESENT;
		psd->dacl = NULL;
	}
	if (!(security_info_wanted & SECINFO_SACL)) {
		psd->type &= ~SEC_DESC_SACL_PRESENT;
		psd->sacl = NULL;
	}

	/* If the SACL/DACL is NULL, but was requested, we mark that it is
	 * present in the reply to match Windows behavior */
	if (psd->sacl == NULL &&
	    security_info_wanted & SACL_SECURITY_INFORMATION)
		psd->type |= SEC_DESC_SACL_PRESENT;
	if (psd->dacl == NULL &&
	    security_info_wanted & DACL_SECURITY_INFORMATION)
		psd->type |= SEC_DESC_DACL_PRESENT;

	if (security_info_wanted & SECINFO_LABEL) {
		/* Like W2K3 return a null object. */
		psd->owner_sid = NULL;
		psd->group_sid = NULL;
		psd->dacl = NULL;
		psd->sacl = NULL;
		psd->type &= ~(SEC_DESC_DACL_PRESENT|SEC_DESC_SACL_PRESENT);
	}

	sd_size = ndr_size_security_descriptor(psd, NULL, 0);

	DEBUG(3,("call_nt_transact_query_security_desc: sd_size = %lu.\n",(unsigned long)sd_size));

	if (DEBUGLEVEL >= 10) {
		DEBUG(10,("call_nt_transact_query_security_desc for file %s\n",
			  fsp_str_dbg(fsp)));
		NDR_PRINT_DEBUG(security_descriptor, psd);
	}

	SIVAL(params,0,(uint32)sd_size);

	if (max_data_count < sd_size) {
		send_nt_replies(conn, req, NT_STATUS_BUFFER_TOO_SMALL,
				params, 4, *ppdata, 0);
		return;
	}

	/*
	 * Allocate the data we will point this at.
	 */

	data = nttrans_realloc(ppdata, sd_size);
	if(data == NULL) {
		reply_nterror(req, NT_STATUS_NO_MEMORY);
		return;
	}

	status = marshall_sec_desc(talloc_tos(), psd,
				   &blob.data, &blob.length);

	if (!NT_STATUS_IS_OK(status)) {
		reply_nterror(req, status);
		return;
	}

	SMB_ASSERT(sd_size == blob.length);
	memcpy(data, blob.data, sd_size);

	send_nt_replies(conn, req, NT_STATUS_OK, params, 4, data, (int)sd_size);

	return;
}