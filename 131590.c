static int chmod_acl_internals( connection_struct *conn, SMB_ACL_T posix_acl, mode_t mode)
{
	int entry_id = SMB_ACL_FIRST_ENTRY;
	SMB_ACL_ENTRY_T entry;
	int num_entries = 0;

	while ( SMB_VFS_SYS_ACL_GET_ENTRY(conn, posix_acl, entry_id, &entry) == 1) {
		SMB_ACL_TAG_T tagtype;
		SMB_ACL_PERMSET_T permset;
		mode_t perms;

		/* get_next... */
		if (entry_id == SMB_ACL_FIRST_ENTRY)
			entry_id = SMB_ACL_NEXT_ENTRY;

		if (SMB_VFS_SYS_ACL_GET_TAG_TYPE(conn, entry, &tagtype) == -1)
			return -1;

		if (SMB_VFS_SYS_ACL_GET_PERMSET(conn, entry, &permset) == -1)
			return -1;

		num_entries++;

		switch(tagtype) {
			case SMB_ACL_USER_OBJ:
				perms = unix_perms_to_acl_perms(mode, S_IRUSR, S_IWUSR, S_IXUSR);
				break;
			case SMB_ACL_GROUP_OBJ:
				perms = unix_perms_to_acl_perms(mode, S_IRGRP, S_IWGRP, S_IXGRP);
				break;
			case SMB_ACL_MASK:
				/*
				 * FIXME: The ACL_MASK entry permissions should really be set to
				 * the union of the permissions of all ACL_USER,
				 * ACL_GROUP_OBJ, and ACL_GROUP entries. That's what
				 * acl_calc_mask() does, but Samba ACLs doesn't provide it.
				 */
				perms = S_IRUSR|S_IWUSR|S_IXUSR;
				break;
			case SMB_ACL_OTHER:
				perms = unix_perms_to_acl_perms(mode, S_IROTH, S_IWOTH, S_IXOTH);
				break;
			default:
				continue;
		}

		if (map_acl_perms_to_permset(conn, perms, &permset) == -1)
			return -1;

		if (SMB_VFS_SYS_ACL_SET_PERMSET(conn, entry, permset) == -1)
			return -1;
	}

	/*
	 * If this is a simple 3 element ACL or no elements then it's a standard
	 * UNIX permission set. Just use chmod...	
	 */

	if ((num_entries == 3) || (num_entries == 0))
		return -1;

	return 0;
}