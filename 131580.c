static void check_owning_objs(canon_ace *ace, DOM_SID *pfile_owner_sid, DOM_SID *pfile_grp_sid)
{
	bool got_user_obj, got_group_obj;
	canon_ace *current_ace;
	int i, entries;

	entries = count_canon_ace_list(ace);
	got_user_obj = False;
	got_group_obj = False;

	for (i=0, current_ace = ace; i < entries; i++, current_ace = current_ace->next) {
		if (current_ace->type == SMB_ACL_USER_OBJ)
			got_user_obj = True;
		else if (current_ace->type == SMB_ACL_GROUP_OBJ)
			got_group_obj = True;
	}
	if (got_user_obj && got_group_obj) {
		DEBUG(10,("check_owning_objs: ACL had owning user/group entries.\n"));
		return;
	}

	for (i=0, current_ace = ace; i < entries; i++, current_ace = current_ace->next) {
		if (!got_user_obj && current_ace->owner_type == UID_ACE &&
				sid_equal(&current_ace->trustee, pfile_owner_sid)) {
			current_ace->type = SMB_ACL_USER_OBJ;
			got_user_obj = True;
		}
		if (!got_group_obj && current_ace->owner_type == GID_ACE &&
				sid_equal(&current_ace->trustee, pfile_grp_sid)) {
			current_ace->type = SMB_ACL_GROUP_OBJ;
			got_group_obj = True;
		}
	}
	if (!got_user_obj)
		DEBUG(10,("check_owning_objs: ACL is missing an owner entry.\n"));
	if (!got_group_obj)
		DEBUG(10,("check_owning_objs: ACL is missing an owning group entry.\n"));
}