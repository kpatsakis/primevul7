static struct canon_ace *canon_ace_entry_for(struct canon_ace *list, SMB_ACL_TAG_T type, posix_id *id)
{
	while (list) {
		if (list->type == type && ((type != SMB_ACL_USER && type != SMB_ACL_GROUP) ||
				(type == SMB_ACL_USER  && id && id->uid == list->unix_ug.uid) ||
				(type == SMB_ACL_GROUP && id && id->gid == list->unix_ug.gid)))
			break;
		list = list->next;
	}
	return list;
}