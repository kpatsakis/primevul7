static NTSTATUS set_sd(files_struct *fsp, uint8 *data, uint32 sd_len,
		       uint32 security_info_sent)
{
	SEC_DESC *psd = NULL;
	NTSTATUS status;

	if (sd_len == 0) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	if (!CAN_WRITE(fsp->conn)) {
		return NT_STATUS_ACCESS_DENIED;
	}

	if (!lp_nt_acl_support(SNUM(fsp->conn))) {
		return NT_STATUS_OK;
	}

	status = unmarshall_sec_desc(talloc_tos(), data, sd_len, &psd);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (psd->owner_sid == NULL) {
		security_info_sent &= ~OWNER_SECURITY_INFORMATION;
	}
	if (psd->group_sid == NULL) {
		security_info_sent &= ~GROUP_SECURITY_INFORMATION;
	}

	/* Ensure we have at least one thing set. */
	if ((security_info_sent & (SECINFO_OWNER|SECINFO_GROUP|SECINFO_DACL|SECINFO_SACL)) == 0) {
		if (security_info_sent & SECINFO_LABEL) {
			/* Only consider SECINFO_LABEL if no other
			   bits are set. Just like W2K3 we don't
			   store this. */
			return NT_STATUS_OK;
		}
		return NT_STATUS_INVALID_PARAMETER;
	}

	/* Ensure we have the rights to do this. */
	if (security_info_sent & SECINFO_OWNER) {
		if (!(fsp->access_mask & SEC_STD_WRITE_OWNER)) {
			return NT_STATUS_ACCESS_DENIED;
		}
	}

	if (security_info_sent & SECINFO_GROUP) {
		if (!(fsp->access_mask & SEC_STD_WRITE_OWNER)) {
			return NT_STATUS_ACCESS_DENIED;
		}
	}

	if (security_info_sent & SECINFO_DACL) {
		if (!(fsp->access_mask & SEC_STD_WRITE_DAC)) {
			return NT_STATUS_ACCESS_DENIED;
		}
		/* Convert all the generic bits. */
		if (psd->dacl) {
			security_acl_map_generic(psd->dacl, &file_generic_mapping);
		}
	}

	if (security_info_sent & SECINFO_SACL) {
		if (!(fsp->access_mask & SEC_FLAG_SYSTEM_SECURITY)) {
			return NT_STATUS_ACCESS_DENIED;
		}
		/* Convert all the generic bits. */
		if (psd->sacl) {
			security_acl_map_generic(psd->sacl, &file_generic_mapping);
		}
	}

	if (DEBUGLEVEL >= 10) {
		DEBUG(10,("set_sd for file %s\n", fsp_str_dbg(fsp)));
		NDR_PRINT_DEBUG(security_descriptor, psd);
	}

	status = SMB_VFS_FSET_NT_ACL(fsp, security_info_sent, psd);

	TALLOC_FREE(psd);

	return status;
}