static SMB_ACL_T create_posix_acl_from_wire(connection_struct *conn, uint16 num_acls, const char *pdata)
{
	unsigned int i;
	SMB_ACL_T the_acl = SMB_VFS_SYS_ACL_INIT(conn, num_acls);

	if (the_acl == NULL) {
		return NULL;
	}

	for (i = 0; i < num_acls; i++) {
		SMB_ACL_ENTRY_T the_entry;
		SMB_ACL_PERMSET_T the_permset;
		SMB_ACL_TAG_T tag_type;

		if (SMB_VFS_SYS_ACL_CREATE_ENTRY(conn, &the_acl, &the_entry) == -1) {
			DEBUG(0,("create_posix_acl_from_wire: Failed to create entry %u. (%s)\n",
				i, strerror(errno) ));
			goto fail;
		}

		if (!unix_ex_wire_to_tagtype(CVAL(pdata,(i*SMB_POSIX_ACL_ENTRY_SIZE)), &tag_type)) {
			DEBUG(0,("create_posix_acl_from_wire: invalid wire tagtype %u on entry %u.\n",
				CVAL(pdata,(i*SMB_POSIX_ACL_ENTRY_SIZE)), i ));
			goto fail;
		}

		if (SMB_VFS_SYS_ACL_SET_TAG_TYPE(conn, the_entry, tag_type) == -1) {
			DEBUG(0,("create_posix_acl_from_wire: Failed to set tagtype on entry %u. (%s)\n",
				i, strerror(errno) ));
			goto fail;
		}

		/* Get the permset pointer from the new ACL entry. */
		if (SMB_VFS_SYS_ACL_GET_PERMSET(conn, the_entry, &the_permset) == -1) {
			DEBUG(0,("create_posix_acl_from_wire: Failed to get permset on entry %u. (%s)\n",
                                i, strerror(errno) ));
                        goto fail;
                }

		/* Map from wire to permissions. */
		if (!unix_ex_wire_to_permset(conn, CVAL(pdata,(i*SMB_POSIX_ACL_ENTRY_SIZE)+1), &the_permset)) {
			DEBUG(0,("create_posix_acl_from_wire: invalid permset %u on entry %u.\n",
				CVAL(pdata,(i*SMB_POSIX_ACL_ENTRY_SIZE) + 1), i ));
			goto fail;
		}

		/* Now apply to the new ACL entry. */
		if (SMB_VFS_SYS_ACL_SET_PERMSET(conn, the_entry, the_permset) == -1) {
			DEBUG(0,("create_posix_acl_from_wire: Failed to add permset on entry %u. (%s)\n",
				i, strerror(errno) ));
			goto fail;
		}

		if (tag_type == SMB_ACL_USER) {
			uint32 uidval = IVAL(pdata,(i*SMB_POSIX_ACL_ENTRY_SIZE)+2);
			uid_t uid = (uid_t)uidval;
			if (SMB_VFS_SYS_ACL_SET_QUALIFIER(conn, the_entry,(void *)&uid) == -1) {
				DEBUG(0,("create_posix_acl_from_wire: Failed to set uid %u on entry %u. (%s)\n",
					(unsigned int)uid, i, strerror(errno) ));
				goto fail;
			}
		}

		if (tag_type == SMB_ACL_GROUP) {
			uint32 gidval = IVAL(pdata,(i*SMB_POSIX_ACL_ENTRY_SIZE)+2);
			gid_t gid = (uid_t)gidval;
			if (SMB_VFS_SYS_ACL_SET_QUALIFIER(conn, the_entry,(void *)&gid) == -1) {
				DEBUG(0,("create_posix_acl_from_wire: Failed to set gid %u on entry %u. (%s)\n",
					(unsigned int)gid, i, strerror(errno) ));
				goto fail;
			}
		}
	}

	return the_acl;

 fail:

	if (the_acl != NULL) {
		SMB_VFS_SYS_ACL_FREE_ACL(conn, the_acl);
	}
	return NULL;
}