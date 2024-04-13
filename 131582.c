static bool ensure_canon_entry_valid(canon_ace **pp_ace,
				     const struct share_params *params,
				     const bool is_directory,
							const DOM_SID *pfile_owner_sid,
							const DOM_SID *pfile_grp_sid,
							const SMB_STRUCT_STAT *pst,
							bool setting_acl)
{
	canon_ace *pace;
	bool got_user = False;
	bool got_grp = False;
	bool got_other = False;
	canon_ace *pace_other = NULL;

	for (pace = *pp_ace; pace; pace = pace->next) {
		if (pace->type == SMB_ACL_USER_OBJ) {

			if (setting_acl)
				apply_default_perms(params, is_directory, pace, S_IRUSR);
			got_user = True;

		} else if (pace->type == SMB_ACL_GROUP_OBJ) {

			/*
			 * Ensure create mask/force create mode is respected on set.
			 */

			if (setting_acl)
				apply_default_perms(params, is_directory, pace, S_IRGRP);
			got_grp = True;

		} else if (pace->type == SMB_ACL_OTHER) {

			/*
			 * Ensure create mask/force create mode is respected on set.
			 */

			if (setting_acl)
				apply_default_perms(params, is_directory, pace, S_IROTH);
			got_other = True;
			pace_other = pace;
		}
	}

	if (!got_user) {
		if ((pace = SMB_MALLOC_P(canon_ace)) == NULL) {
			DEBUG(0,("ensure_canon_entry_valid: malloc fail.\n"));
			return False;
		}

		ZERO_STRUCTP(pace);
		pace->type = SMB_ACL_USER_OBJ;
		pace->owner_type = UID_ACE;
		pace->unix_ug.uid = pst->st_uid;
		pace->trustee = *pfile_owner_sid;
		pace->attr = ALLOW_ACE;

		if (setting_acl) {
			/* See if the owning user is in any of the other groups in
			   the ACE. If so, OR in the permissions from that group. */

			bool group_matched = False;
			canon_ace *pace_iter;

			for (pace_iter = *pp_ace; pace_iter; pace_iter = pace_iter->next) {
				if (pace_iter->type == SMB_ACL_GROUP_OBJ || pace_iter->type == SMB_ACL_GROUP) {
					if (uid_entry_in_group(pace, pace_iter)) {
						pace->perms |= pace_iter->perms;
						group_matched = True;
					}
				}
			}

			/* If we only got an "everyone" perm, just use that. */
			if (!group_matched) {
				if (got_other)
					pace->perms = pace_other->perms;
				else
					pace->perms = 0;
			}

			apply_default_perms(params, is_directory, pace, S_IRUSR);
		} else {
			pace->perms = unix_perms_to_acl_perms(pst->st_mode, S_IRUSR, S_IWUSR, S_IXUSR);
		}

		DLIST_ADD(*pp_ace, pace);
	}

	if (!got_grp) {
		if ((pace = SMB_MALLOC_P(canon_ace)) == NULL) {
			DEBUG(0,("ensure_canon_entry_valid: malloc fail.\n"));
			return False;
		}

		ZERO_STRUCTP(pace);
		pace->type = SMB_ACL_GROUP_OBJ;
		pace->owner_type = GID_ACE;
		pace->unix_ug.uid = pst->st_gid;
		pace->trustee = *pfile_grp_sid;
		pace->attr = ALLOW_ACE;
		if (setting_acl) {
			/* If we only got an "everyone" perm, just use that. */
			if (got_other)
				pace->perms = pace_other->perms;
			else
				pace->perms = 0;
			apply_default_perms(params, is_directory, pace, S_IRGRP);
		} else {
			pace->perms = unix_perms_to_acl_perms(pst->st_mode, S_IRGRP, S_IWGRP, S_IXGRP);
		}

		DLIST_ADD(*pp_ace, pace);
	}

	if (!got_other) {
		if ((pace = SMB_MALLOC_P(canon_ace)) == NULL) {
			DEBUG(0,("ensure_canon_entry_valid: malloc fail.\n"));
			return False;
		}

		ZERO_STRUCTP(pace);
		pace->type = SMB_ACL_OTHER;
		pace->owner_type = WORLD_ACE;
		pace->unix_ug.world = -1;
		pace->trustee = global_sid_World;
		pace->attr = ALLOW_ACE;
		if (setting_acl) {
			pace->perms = 0;
			apply_default_perms(params, is_directory, pace, S_IROTH);
		} else
			pace->perms = unix_perms_to_acl_perms(pst->st_mode, S_IROTH, S_IWOTH, S_IXOTH);

		DLIST_ADD(*pp_ace, pace);
	}

	return True;
}