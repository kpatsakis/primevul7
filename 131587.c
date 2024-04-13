static bool remove_posix_acl(connection_struct *conn, files_struct *fsp, const char *fname)
{
	SMB_ACL_T file_acl = NULL;
	int entry_id = SMB_ACL_FIRST_ENTRY;
	SMB_ACL_ENTRY_T entry;
	bool ret = False;
	/* Create a new ACL with only 3 entries, u/g/w. */
	SMB_ACL_T new_file_acl = SMB_VFS_SYS_ACL_INIT(conn, 3);
	SMB_ACL_ENTRY_T user_ent = NULL;
	SMB_ACL_ENTRY_T group_ent = NULL;
	SMB_ACL_ENTRY_T other_ent = NULL;

	if (new_file_acl == NULL) {
		DEBUG(5,("remove_posix_acl: failed to init new ACL with 3 entries for file %s.\n", fname));
		return False;
	}

	/* Now create the u/g/w entries. */
	if (SMB_VFS_SYS_ACL_CREATE_ENTRY(conn, &new_file_acl, &user_ent) == -1) {
		DEBUG(5,("remove_posix_acl: Failed to create user entry for file %s. (%s)\n",
			fname, strerror(errno) ));
		goto done;
	}
	if (SMB_VFS_SYS_ACL_SET_TAG_TYPE(conn, user_ent, SMB_ACL_USER_OBJ) == -1) {
		DEBUG(5,("remove_posix_acl: Failed to set user entry for file %s. (%s)\n",
			fname, strerror(errno) ));
		goto done;
	}

	if (SMB_VFS_SYS_ACL_CREATE_ENTRY(conn, &new_file_acl, &group_ent) == -1) {
		DEBUG(5,("remove_posix_acl: Failed to create group entry for file %s. (%s)\n",
			fname, strerror(errno) ));
		goto done;
	}
	if (SMB_VFS_SYS_ACL_SET_TAG_TYPE(conn, group_ent, SMB_ACL_GROUP_OBJ) == -1) {
		DEBUG(5,("remove_posix_acl: Failed to set group entry for file %s. (%s)\n",
			fname, strerror(errno) ));
		goto done;
	}

	if (SMB_VFS_SYS_ACL_CREATE_ENTRY(conn, &new_file_acl, &other_ent) == -1) {
		DEBUG(5,("remove_posix_acl: Failed to create other entry for file %s. (%s)\n",
			fname, strerror(errno) ));
		goto done;
	}
	if (SMB_VFS_SYS_ACL_SET_TAG_TYPE(conn, other_ent, SMB_ACL_OTHER) == -1) {
		DEBUG(5,("remove_posix_acl: Failed to set other entry for file %s. (%s)\n",
			fname, strerror(errno) ));
		goto done;
	}

	/* Get the current file ACL. */
	if (fsp && fsp->fh->fd != -1) {
		file_acl = SMB_VFS_SYS_ACL_GET_FD(fsp);
	} else {
		file_acl = SMB_VFS_SYS_ACL_GET_FILE( conn, fname, SMB_ACL_TYPE_ACCESS);
	}

	if (file_acl == NULL) {
		/* This is only returned if an error occurred. Even for a file with
		   no acl a u/g/w acl should be returned. */
		DEBUG(5,("remove_posix_acl: failed to get ACL from file %s (%s).\n",
			fname, strerror(errno) ));
		goto done;
	}

	while ( SMB_VFS_SYS_ACL_GET_ENTRY(conn, file_acl, entry_id, &entry) == 1) {
		SMB_ACL_TAG_T tagtype;
		SMB_ACL_PERMSET_T permset;

		/* get_next... */
		if (entry_id == SMB_ACL_FIRST_ENTRY)
			entry_id = SMB_ACL_NEXT_ENTRY;

		if (SMB_VFS_SYS_ACL_GET_TAG_TYPE(conn, entry, &tagtype) == -1) {
			DEBUG(5,("remove_posix_acl: failed to get tagtype from ACL on file %s (%s).\n",
				fname, strerror(errno) ));
			goto done;
		}

		if (SMB_VFS_SYS_ACL_GET_PERMSET(conn, entry, &permset) == -1) {
			DEBUG(5,("remove_posix_acl: failed to get permset from ACL on file %s (%s).\n",
				fname, strerror(errno) ));
			goto done;
		}

		if (tagtype == SMB_ACL_USER_OBJ) {
			if (SMB_VFS_SYS_ACL_SET_PERMSET(conn, user_ent, permset) == -1) {
				DEBUG(5,("remove_posix_acl: failed to set permset from ACL on file %s (%s).\n",
					fname, strerror(errno) ));
			}
		} else if (tagtype == SMB_ACL_GROUP_OBJ) {
			if (SMB_VFS_SYS_ACL_SET_PERMSET(conn, group_ent, permset) == -1) {
				DEBUG(5,("remove_posix_acl: failed to set permset from ACL on file %s (%s).\n",
					fname, strerror(errno) ));
			}
		} else if (tagtype == SMB_ACL_OTHER) {
			if (SMB_VFS_SYS_ACL_SET_PERMSET(conn, other_ent, permset) == -1) {
				DEBUG(5,("remove_posix_acl: failed to set permset from ACL on file %s (%s).\n",
					fname, strerror(errno) ));
			}
		}
	}

	/* Set the new empty file ACL. */
	if (fsp && fsp->fh->fd != -1) {
		if (SMB_VFS_SYS_ACL_SET_FD(fsp, new_file_acl) == -1) {
			DEBUG(5,("remove_posix_acl: acl_set_file failed on %s (%s)\n",
				fname, strerror(errno) ));
			goto done;
		}
	} else {
		if (SMB_VFS_SYS_ACL_SET_FILE(conn, fname, SMB_ACL_TYPE_ACCESS, new_file_acl) == -1) {
			DEBUG(5,("remove_posix_acl: acl_set_file failed on %s (%s)\n",
				fname, strerror(errno) ));
			goto done;
		}
	}

	ret = True;

 done:

	if (file_acl) {
		SMB_VFS_SYS_ACL_FREE_ACL(conn, file_acl);
	}
	if (new_file_acl) {
		SMB_VFS_SYS_ACL_FREE_ACL(conn, new_file_acl);
	}
	return ret;
}