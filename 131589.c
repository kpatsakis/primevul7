static canon_ace *canonicalise_acl(struct connection_struct *conn,
				   const char *fname, SMB_ACL_T posix_acl,
				   const SMB_STRUCT_STAT *psbuf,
				   const DOM_SID *powner, const DOM_SID *pgroup, struct pai_val *pal, SMB_ACL_TYPE_T the_acl_type)
{
	mode_t acl_mask = (S_IRUSR|S_IWUSR|S_IXUSR);
	canon_ace *list_head = NULL;
	canon_ace *ace = NULL;
	canon_ace *next_ace = NULL;
	int entry_id = SMB_ACL_FIRST_ENTRY;
	SMB_ACL_ENTRY_T entry;
	size_t ace_count;

	while ( posix_acl && (SMB_VFS_SYS_ACL_GET_ENTRY(conn, posix_acl, entry_id, &entry) == 1)) {
		SMB_ACL_TAG_T tagtype;
		SMB_ACL_PERMSET_T permset;
		DOM_SID sid;
		posix_id unix_ug;
		enum ace_owner owner_type;

		/* get_next... */
		if (entry_id == SMB_ACL_FIRST_ENTRY)
			entry_id = SMB_ACL_NEXT_ENTRY;

		/* Is this a MASK entry ? */
		if (SMB_VFS_SYS_ACL_GET_TAG_TYPE(conn, entry, &tagtype) == -1)
			continue;

		if (SMB_VFS_SYS_ACL_GET_PERMSET(conn, entry, &permset) == -1)
			continue;

		/* Decide which SID to use based on the ACL type. */
		switch(tagtype) {
			case SMB_ACL_USER_OBJ:
				/* Get the SID from the owner. */
				sid_copy(&sid, powner);
				unix_ug.uid = psbuf->st_uid;
				owner_type = UID_ACE;
				break;
			case SMB_ACL_USER:
				{
					uid_t *puid = (uid_t *)SMB_VFS_SYS_ACL_GET_QUALIFIER(conn, entry);
					if (puid == NULL) {
						DEBUG(0,("canonicalise_acl: Failed to get uid.\n"));
						continue;
					}
					/*
					 * A SMB_ACL_USER entry for the owner is shadowed by the
					 * SMB_ACL_USER_OBJ entry and Windows also cannot represent
					 * that entry, so we ignore it. We also don't create such
					 * entries out of the blue when setting ACLs, so a get/set
					 * cycle will drop them.
					 */
					if (the_acl_type == SMB_ACL_TYPE_ACCESS && *puid == psbuf->st_uid) {
						SMB_VFS_SYS_ACL_FREE_QUALIFIER(conn, (void *)puid,tagtype);
						continue;
					}
					uid_to_sid( &sid, *puid);
					unix_ug.uid = *puid;
					owner_type = UID_ACE;
					SMB_VFS_SYS_ACL_FREE_QUALIFIER(conn, (void *)puid,tagtype);
					break;
				}
			case SMB_ACL_GROUP_OBJ:
				/* Get the SID from the owning group. */
				sid_copy(&sid, pgroup);
				unix_ug.gid = psbuf->st_gid;
				owner_type = GID_ACE;
				break;
			case SMB_ACL_GROUP:
				{
					gid_t *pgid = (gid_t *)SMB_VFS_SYS_ACL_GET_QUALIFIER(conn, entry);
					if (pgid == NULL) {
						DEBUG(0,("canonicalise_acl: Failed to get gid.\n"));
						continue;
					}
					gid_to_sid( &sid, *pgid);
					unix_ug.gid = *pgid;
					owner_type = GID_ACE;
					SMB_VFS_SYS_ACL_FREE_QUALIFIER(conn, (void *)pgid,tagtype);
					break;
				}
			case SMB_ACL_MASK:
				acl_mask = convert_permset_to_mode_t(conn, permset);
				continue; /* Don't count the mask as an entry. */
			case SMB_ACL_OTHER:
				/* Use the Everyone SID */
				sid = global_sid_World;
				unix_ug.world = -1;
				owner_type = WORLD_ACE;
				break;
			default:
				DEBUG(0,("canonicalise_acl: Unknown tagtype %u\n", (unsigned int)tagtype));
				continue;
		}

		/*
		 * Add this entry to the list.
		 */

		if ((ace = SMB_MALLOC_P(canon_ace)) == NULL)
			goto fail;

		ZERO_STRUCTP(ace);
		ace->type = tagtype;
		ace->perms = convert_permset_to_mode_t(conn, permset);
		ace->attr = ALLOW_ACE;
		ace->trustee = sid;
		ace->unix_ug = unix_ug;
		ace->owner_type = owner_type;
		ace->inherited = get_inherited_flag(pal, ace, (the_acl_type == SMB_ACL_TYPE_DEFAULT));

		DLIST_ADD(list_head, ace);
	}

	/*
	 * This next call will ensure we have at least a user/group/world set.
	 */

	if (!ensure_canon_entry_valid(&list_head, conn->params,
				      S_ISDIR(psbuf->st_mode), powner, pgroup,
				      psbuf, False))
		goto fail;

	/*
	 * Now go through the list, masking the permissions with the
	 * acl_mask. Ensure all DENY Entries are at the start of the list.
	 */

	DEBUG(10,("canonicalise_acl: %s ace entries before arrange :\n", the_acl_type == SMB_ACL_TYPE_ACCESS ? "Access" : "Default" ));

	for ( ace_count = 0, ace = list_head; ace; ace = next_ace, ace_count++) {
		next_ace = ace->next;

		/* Masks are only applied to entries other than USER_OBJ and OTHER. */
		if (ace->type != SMB_ACL_OTHER && ace->type != SMB_ACL_USER_OBJ)
			ace->perms &= acl_mask;

		if (ace->perms == 0) {
			DLIST_PROMOTE(list_head, ace);
		}

		if( DEBUGLVL( 10 ) ) {
			print_canon_ace(ace, ace_count);
		}
	}

	arrange_posix_perms(fname,&list_head );

	print_canon_ace_list( "canonicalise_acl: ace entries after arrange", list_head );

	return list_head;

  fail:

	free_canon_ace_list(list_head);
	return NULL;
}