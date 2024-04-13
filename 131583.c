static void arrange_posix_perms(const char *filename, canon_ace **pp_list_head)
{
	canon_ace *list_head = *pp_list_head;
	canon_ace *owner_ace = NULL;
	canon_ace *other_ace = NULL;
	canon_ace *ace = NULL;

	for (ace = list_head; ace; ace = ace->next) {
		if (ace->type == SMB_ACL_USER_OBJ)
			owner_ace = ace;
		else if (ace->type == SMB_ACL_OTHER) {
			/* Last ace - this is "other" */
			other_ace = ace;
		}
	}
		
	if (!owner_ace || !other_ace) {
		DEBUG(0,("arrange_posix_perms: Invalid POSIX permissions for file %s, missing owner or other.\n",
			filename ));
		return;
	}

	/*
	 * The POSIX algorithm applies to owner first, and other last,
	 * so ensure they are arranged in this order.
	 */

	if (owner_ace) {
		DLIST_PROMOTE(list_head, owner_ace);
	}

	if (other_ace) {
		DLIST_DEMOTE(list_head, other_ace, canon_ace *);
	}

	/* We have probably changed the head of the list. */

	*pp_list_head = list_head;
}