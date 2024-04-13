static bool create_canon_ace_lists(files_struct *fsp, SMB_STRUCT_STAT *pst,
							DOM_SID *pfile_owner_sid,
							DOM_SID *pfile_grp_sid,
							canon_ace **ppfile_ace, canon_ace **ppdir_ace,
							SEC_ACL *dacl)
{
	bool all_aces_are_inherit_only = (fsp->is_directory ? True : False);
	canon_ace *file_ace = NULL;
	canon_ace *dir_ace = NULL;
	canon_ace *current_ace = NULL;
	bool got_dir_allow = False;
	bool got_file_allow = False;
	int i, j;

	*ppfile_ace = NULL;
	*ppdir_ace = NULL;

	/*
	 * Convert the incoming ACL into a more regular form.
	 */

	for(i = 0; i < dacl->num_aces; i++) {
		SEC_ACE *psa = &dacl->aces[i];

		if((psa->type != SEC_ACE_TYPE_ACCESS_ALLOWED) && (psa->type != SEC_ACE_TYPE_ACCESS_DENIED)) {
			DEBUG(3,("create_canon_ace_lists: unable to set anything but an ALLOW or DENY ACE.\n"));
			return False;
		}

		if (nt4_compatible_acls()) {
			/*
			 * The security mask may be UNIX_ACCESS_NONE which should map into
			 * no permissions (we overload the WRITE_OWNER bit for this) or it
			 * should be one of the ALL/EXECUTE/READ/WRITE bits. Arrange for this
			 * to be so. Any other bits override the UNIX_ACCESS_NONE bit.
			 */

			/*
			 * Convert GENERIC bits to specific bits.
			 */
 
			se_map_generic(&psa->access_mask, &file_generic_mapping);

			psa->access_mask &= (UNIX_ACCESS_NONE|FILE_ALL_ACCESS);

			if(psa->access_mask != UNIX_ACCESS_NONE)
				psa->access_mask &= ~UNIX_ACCESS_NONE;
		}
	}

	/*
	 * Deal with the fact that NT 4.x re-writes the canonical format
	 * that we return for default ACLs. If a directory ACE is identical
	 * to a inherited directory ACE then NT changes the bits so that the
	 * first ACE is set to OI|IO and the second ACE for this SID is set
	 * to CI. We need to repair this. JRA.
	 */

	for(i = 0; i < dacl->num_aces; i++) {
		SEC_ACE *psa1 = &dacl->aces[i];

		for (j = i + 1; j < dacl->num_aces; j++) {
			SEC_ACE *psa2 = &dacl->aces[j];

			if (psa1->access_mask != psa2->access_mask)
				continue;

			if (!sid_equal(&psa1->trustee, &psa2->trustee))
				continue;

			/*
			 * Ok - permission bits and SIDs are equal.
			 * Check if flags were re-written.
			 */

			if (psa1->flags & SEC_ACE_FLAG_INHERIT_ONLY) {

				psa1->flags |= (psa2->flags & (SEC_ACE_FLAG_CONTAINER_INHERIT|SEC_ACE_FLAG_OBJECT_INHERIT));
				psa2->flags &= ~(SEC_ACE_FLAG_CONTAINER_INHERIT|SEC_ACE_FLAG_OBJECT_INHERIT);

			} else if (psa2->flags & SEC_ACE_FLAG_INHERIT_ONLY) {

				psa2->flags |= (psa1->flags & (SEC_ACE_FLAG_CONTAINER_INHERIT|SEC_ACE_FLAG_OBJECT_INHERIT));
				psa1->flags &= ~(SEC_ACE_FLAG_CONTAINER_INHERIT|SEC_ACE_FLAG_OBJECT_INHERIT);

			}
		}
	}

	for(i = 0; i < dacl->num_aces; i++) {
		SEC_ACE *psa = &dacl->aces[i];

		/*
		 * Create a cannon_ace entry representing this NT DACL ACE.
		 */

		if ((current_ace = SMB_MALLOC_P(canon_ace)) == NULL) {
			free_canon_ace_list(file_ace);
			free_canon_ace_list(dir_ace);
			DEBUG(0,("create_canon_ace_lists: malloc fail.\n"));
			return False;
		}

		ZERO_STRUCTP(current_ace);

		sid_copy(&current_ace->trustee, &psa->trustee);

		/*
		 * Try and work out if the SID is a user or group
		 * as we need to flag these differently for POSIX.
		 * Note what kind of a POSIX ACL this should map to.
		 */

		if( sid_equal(&current_ace->trustee, &global_sid_World)) {
			current_ace->owner_type = WORLD_ACE;
			current_ace->unix_ug.world = -1;
			current_ace->type = SMB_ACL_OTHER;
		} else if (sid_equal(&current_ace->trustee, &global_sid_Creator_Owner)) {
			current_ace->owner_type = UID_ACE;
			current_ace->unix_ug.uid = pst->st_uid;
			current_ace->type = SMB_ACL_USER_OBJ;

			/*
			 * The Creator Owner entry only specifies inheritable permissions,
			 * never access permissions. WinNT doesn't always set the ACE to
			 *INHERIT_ONLY, though.
			 */

			if (nt4_compatible_acls())
				psa->flags |= SEC_ACE_FLAG_INHERIT_ONLY;
		} else if (sid_equal(&current_ace->trustee, &global_sid_Creator_Group)) {
			current_ace->owner_type = GID_ACE;
			current_ace->unix_ug.gid = pst->st_gid;
			current_ace->type = SMB_ACL_GROUP_OBJ;

			/*
			 * The Creator Group entry only specifies inheritable permissions,
			 * never access permissions. WinNT doesn't always set the ACE to
			 *INHERIT_ONLY, though.
			 */
			if (nt4_compatible_acls())
				psa->flags |= SEC_ACE_FLAG_INHERIT_ONLY;

		} else if (sid_to_uid( &current_ace->trustee, &current_ace->unix_ug.uid)) {
			current_ace->owner_type = UID_ACE;
			/* If it's the owning user, this is a user_obj, not
			 * a user. */
			if (current_ace->unix_ug.uid == pst->st_uid) {
				current_ace->type = SMB_ACL_USER_OBJ;
			} else {
				current_ace->type = SMB_ACL_USER;
			}
		} else if (sid_to_gid( &current_ace->trustee, &current_ace->unix_ug.gid)) {
			current_ace->owner_type = GID_ACE;
			/* If it's the primary group, this is a group_obj, not
			 * a group. */
			if (current_ace->unix_ug.gid == pst->st_gid) {
				current_ace->type = SMB_ACL_GROUP_OBJ;
			} else {
				current_ace->type = SMB_ACL_GROUP;
			}
		} else {
			/*
			 * Silently ignore map failures in non-mappable SIDs (NT Authority, BUILTIN etc).
			 */

			if (non_mappable_sid(&psa->trustee)) {
				DEBUG(10, ("create_canon_ace_lists: ignoring "
					   "non-mappable SID %s\n",
					   sid_string_dbg(&psa->trustee)));
				SAFE_FREE(current_ace);
				continue;
			}

			free_canon_ace_list(file_ace);
			free_canon_ace_list(dir_ace);
			DEBUG(0, ("create_canon_ace_lists: unable to map SID "
				  "%s to uid or gid.\n",
				  sid_string_dbg(&current_ace->trustee)));
			SAFE_FREE(current_ace);
			return False;
		}

		/*
		 * Map the given NT permissions into a UNIX mode_t containing only
		 * S_I(R|W|X)USR bits.
		 */

		current_ace->perms |= map_nt_perms( &psa->access_mask, S_IRUSR);
		current_ace->attr = (psa->type == SEC_ACE_TYPE_ACCESS_ALLOWED) ? ALLOW_ACE : DENY_ACE;
		current_ace->inherited = ((psa->flags & SEC_ACE_FLAG_INHERITED_ACE) ? True : False);

		/*
		 * Now add the created ace to either the file list, the directory
		 * list, or both. We *MUST* preserve the order here (hence we use
		 * DLIST_ADD_END) as NT ACLs are order dependent.
		 */

		if (fsp->is_directory) {

			/*
			 * We can only add to the default POSIX ACE list if the ACE is
			 * designed to be inherited by both files and directories.
			 */

			if ((psa->flags & (SEC_ACE_FLAG_OBJECT_INHERIT|SEC_ACE_FLAG_CONTAINER_INHERIT)) ==
				(SEC_ACE_FLAG_OBJECT_INHERIT|SEC_ACE_FLAG_CONTAINER_INHERIT)) {

				DLIST_ADD_END(dir_ace, current_ace, canon_ace *);

				/*
				 * Note if this was an allow ace. We can't process
				 * any further deny ace's after this.
				 */

				if (current_ace->attr == ALLOW_ACE)
					got_dir_allow = True;

				if ((current_ace->attr == DENY_ACE) && got_dir_allow) {
					DEBUG(0,("create_canon_ace_lists: malformed ACL in inheritable ACL ! \
Deny entry after Allow entry. Failing to set on file %s.\n", fsp->fsp_name ));
					free_canon_ace_list(file_ace);
					free_canon_ace_list(dir_ace);
					return False;
				}	

				if( DEBUGLVL( 10 )) {
					dbgtext("create_canon_ace_lists: adding dir ACL:\n");
					print_canon_ace( current_ace, 0);
				}

				/*
				 * If this is not an inherit only ACE we need to add a duplicate
				 * to the file acl.
				 */

				if (!(psa->flags & SEC_ACE_FLAG_INHERIT_ONLY)) {
					canon_ace *dup_ace = dup_canon_ace(current_ace);

					if (!dup_ace) {
						DEBUG(0,("create_canon_ace_lists: malloc fail !\n"));
						free_canon_ace_list(file_ace);
						free_canon_ace_list(dir_ace);
						return False;
					}

					/*
					 * We must not free current_ace here as its
					 * pointer is now owned by the dir_ace list.
					 */
					current_ace = dup_ace;
				} else {
					/*
					 * We must not free current_ace here as its
					 * pointer is now owned by the dir_ace list.
					 */
					current_ace = NULL;
				}
			}
		}

		/*
		 * Only add to the file ACL if not inherit only.
		 */

		if (current_ace && !(psa->flags & SEC_ACE_FLAG_INHERIT_ONLY)) {
			DLIST_ADD_END(file_ace, current_ace, canon_ace *);

			/*
			 * Note if this was an allow ace. We can't process
			 * any further deny ace's after this.
			 */

			if (current_ace->attr == ALLOW_ACE)
				got_file_allow = True;

			if ((current_ace->attr == DENY_ACE) && got_file_allow) {
				DEBUG(0,("create_canon_ace_lists: malformed ACL in file ACL ! \
Deny entry after Allow entry. Failing to set on file %s.\n", fsp->fsp_name ));
				free_canon_ace_list(file_ace);
				free_canon_ace_list(dir_ace);
				return False;
			}	

			if( DEBUGLVL( 10 )) {
				dbgtext("create_canon_ace_lists: adding file ACL:\n");
				print_canon_ace( current_ace, 0);
			}
			all_aces_are_inherit_only = False;
			/*
			 * We must not free current_ace here as its
			 * pointer is now owned by the file_ace list.
			 */
			current_ace = NULL;
		}

		/*
		 * Free if ACE was not added.
		 */

		SAFE_FREE(current_ace);
	}

	if (fsp->is_directory && all_aces_are_inherit_only) {
		/*
		 * Windows 2000 is doing one of these weird 'inherit acl'
		 * traverses to conserve NTFS ACL resources. Just pretend
		 * there was no DACL sent. JRA.
		 */

		DEBUG(10,("create_canon_ace_lists: Win2k inherit acl traverse. Ignoring DACL.\n"));
		free_canon_ace_list(file_ace);
		free_canon_ace_list(dir_ace);
		file_ace = NULL;
		dir_ace = NULL;
	} else {
		/*
		 * Check if we have SMB_ACL_USER_OBJ and SMB_ACL_GROUP_OBJ entries in each
		 * ACL. If we don't have them, check if any SMB_ACL_USER/SMB_ACL_GROUP
		 * entries can be converted to *_OBJ. Usually we will already have these
		 * entries in the Default ACL, and the Access ACL will not have them.
		 */
		if (file_ace) {
			check_owning_objs(file_ace, pfile_owner_sid, pfile_grp_sid);
		}
		if (dir_ace) {
			check_owning_objs(dir_ace, pfile_owner_sid, pfile_grp_sid);
		}
	}

	*ppfile_ace = file_ace;
	*ppdir_ace = dir_ace;

	return True;
}