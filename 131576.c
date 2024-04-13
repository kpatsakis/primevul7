NTSTATUS unpack_nt_owners(int snum, uid_t *puser, gid_t *pgrp, uint32 security_info_sent, SEC_DESC *psd)
{
	DOM_SID owner_sid;
	DOM_SID grp_sid;

	*puser = (uid_t)-1;
	*pgrp = (gid_t)-1;

	if(security_info_sent == 0) {
		DEBUG(0,("unpack_nt_owners: no security info sent !\n"));
		return NT_STATUS_OK;
	}

	/*
	 * Validate the owner and group SID's.
	 */

	memset(&owner_sid, '\0', sizeof(owner_sid));
	memset(&grp_sid, '\0', sizeof(grp_sid));

	DEBUG(5,("unpack_nt_owners: validating owner_sids.\n"));

	/*
	 * Don't immediately fail if the owner sid cannot be validated.
	 * This may be a group chown only set.
	 */

	if (security_info_sent & OWNER_SECURITY_INFORMATION) {
		sid_copy(&owner_sid, psd->owner_sid);
		if (!sid_to_uid(&owner_sid, puser)) {
			if (lp_force_unknown_acl_user(snum)) {
				/* this allows take ownership to work
				 * reasonably */
				*puser = current_user.ut.uid;
			} else {
				DEBUG(3,("unpack_nt_owners: unable to validate"
					 " owner sid for %s\n",
					 sid_string_dbg(&owner_sid)));
				return NT_STATUS_INVALID_OWNER;
			}
		}
		DEBUG(3,("unpack_nt_owners: owner sid mapped to uid %u\n",
			 (unsigned int)*puser ));
 	}

	/*
	 * Don't immediately fail if the group sid cannot be validated.
	 * This may be an owner chown only set.
	 */

	if (security_info_sent & GROUP_SECURITY_INFORMATION) {
		sid_copy(&grp_sid, psd->group_sid);
		if (!sid_to_gid( &grp_sid, pgrp)) {
			if (lp_force_unknown_acl_user(snum)) {
				/* this allows take group ownership to work
				 * reasonably */
				*pgrp = current_user.ut.gid;
			} else {
				DEBUG(3,("unpack_nt_owners: unable to validate"
					 " group sid.\n"));
				return NT_STATUS_INVALID_OWNER;
			}
		}
		DEBUG(3,("unpack_nt_owners: group sid mapped to gid %u\n",
			 (unsigned int)*pgrp));
 	}

	DEBUG(5,("unpack_nt_owners: owner_sids validated.\n"));

	return NT_STATUS_OK;
}