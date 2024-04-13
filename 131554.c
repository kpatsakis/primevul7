static bool unpack_canon_ace(files_struct *fsp, 
							SMB_STRUCT_STAT *pst,
							DOM_SID *pfile_owner_sid,
							DOM_SID *pfile_grp_sid,
							canon_ace **ppfile_ace, canon_ace **ppdir_ace,
							uint32 security_info_sent, SEC_DESC *psd)
{
	canon_ace *file_ace = NULL;
	canon_ace *dir_ace = NULL;

	*ppfile_ace = NULL;
	*ppdir_ace = NULL;

	if(security_info_sent == 0) {
		DEBUG(0,("unpack_canon_ace: no security info sent !\n"));
		return False;
	}

	/*
	 * If no DACL then this is a chown only security descriptor.
	 */

	if(!(security_info_sent & DACL_SECURITY_INFORMATION) || !psd->dacl)
		return True;

	/*
	 * Now go through the DACL and create the canon_ace lists.
	 */

	if (!create_canon_ace_lists( fsp, pst, pfile_owner_sid, pfile_grp_sid,
								&file_ace, &dir_ace, psd->dacl))
		return False;

	if ((file_ace == NULL) && (dir_ace == NULL)) {
		/* W2K traverse DACL set - ignore. */
		return True;
	}

	/*
	 * Go through the canon_ace list and merge entries
	 * belonging to identical users of identical allow or deny type.
	 * We can do this as all deny entries come first, followed by
	 * all allow entries (we have mandated this before accepting this acl).
	 */

	print_canon_ace_list( "file ace - before merge", file_ace);
	merge_aces( &file_ace );

	print_canon_ace_list( "dir ace - before merge", dir_ace);
	merge_aces( &dir_ace );

	/*
	 * NT ACLs are order dependent. Go through the acl lists and
	 * process DENY entries by masking the allow entries.
	 */

	print_canon_ace_list( "file ace - before deny", file_ace);
	process_deny_list( &file_ace);

	print_canon_ace_list( "dir ace - before deny", dir_ace);
	process_deny_list( &dir_ace);

	/*
	 * A well formed POSIX file or default ACL has at least 3 entries, a 
	 * SMB_ACL_USER_OBJ, SMB_ACL_GROUP_OBJ, SMB_ACL_OTHER_OBJ
	 * and optionally a mask entry. Ensure this is the case.
	 */

	print_canon_ace_list( "file ace - before valid", file_ace);

	/*
	 * A default 3 element mode entry for a file should be r-- --- ---.
	 * A default 3 element mode entry for a directory should be rwx --- ---.
	 */

	pst->st_mode = create_default_mode(fsp, False);

	if (!ensure_canon_entry_valid(&file_ace, fsp->conn->params, fsp->is_directory, pfile_owner_sid, pfile_grp_sid, pst, True)) {
		free_canon_ace_list(file_ace);
		free_canon_ace_list(dir_ace);
		return False;
	}

	print_canon_ace_list( "dir ace - before valid", dir_ace);

	/*
	 * A default inheritable 3 element mode entry for a directory should be the
	 * mode Samba will use to create a file within. Ensure user rwx bits are set if
	 * it's a directory.
	 */

	pst->st_mode = create_default_mode(fsp, True);

	if (dir_ace && !ensure_canon_entry_valid(&dir_ace, fsp->conn->params, fsp->is_directory, pfile_owner_sid, pfile_grp_sid, pst, True)) {
		free_canon_ace_list(file_ace);
		free_canon_ace_list(dir_ace);
		return False;
	}

	print_canon_ace_list( "file ace - return", file_ace);
	print_canon_ace_list( "dir ace - return", dir_ace);

	*ppfile_ace = file_ace;
	*ppdir_ace = dir_ace;
	return True;

}