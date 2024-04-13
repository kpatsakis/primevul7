static void process_deny_list( canon_ace **pp_ace_list )
{
	canon_ace *ace_list = *pp_ace_list;
	canon_ace *curr_ace = NULL;
	canon_ace *curr_ace_next = NULL;

	/* Pass 1 above - look for an Everyone, deny entry. */

	for (curr_ace = ace_list; curr_ace; curr_ace = curr_ace_next) {
		canon_ace *allow_ace_p;

		curr_ace_next = curr_ace->next; /* So we can't lose the link. */

		if (curr_ace->attr != DENY_ACE)
			continue;

		if (curr_ace->perms == (mode_t)0) {

			/* Deny nothing entry - delete. */

			DLIST_REMOVE(ace_list, curr_ace);
			continue;
		}

		if (!sid_equal(&curr_ace->trustee, &global_sid_World))
			continue;

		/* JRATEST - assert. */
		SMB_ASSERT(curr_ace->owner_type == WORLD_ACE);

		if (curr_ace->perms == ALL_ACE_PERMS) {

			/*
			 * Optimisation. This is a DENY_ALL to Everyone. Truncate the
			 * list at this point including this entry.
			 */

			canon_ace *prev_entry = curr_ace->prev;

			free_canon_ace_list( curr_ace );
			if (prev_entry)
				prev_entry->next = NULL;
			else {
				/* We deleted the entire list. */
				ace_list = NULL;
			}
			break;
		}

		for (allow_ace_p = curr_ace->next; allow_ace_p; allow_ace_p = allow_ace_p->next) {

			/* 
			 * Only mask off allow entries.
			 */

			if (allow_ace_p->attr != ALLOW_ACE)
				continue;

			allow_ace_p->perms &= ~curr_ace->perms;
		}

		/*
		 * Now it's been applied, remove it.
		 */

		DLIST_REMOVE(ace_list, curr_ace);
	}

	/* Pass 2 above - deal with deny user entries. */

	for (curr_ace = ace_list; curr_ace; curr_ace = curr_ace_next) {
		mode_t new_perms = (mode_t)0;
		canon_ace *allow_ace_p;

		curr_ace_next = curr_ace->next; /* So we can't lose the link. */

		if (curr_ace->attr != DENY_ACE)
			continue;

		if (curr_ace->owner_type != UID_ACE)
			continue;

		if (curr_ace->perms == ALL_ACE_PERMS) {

			/*
			 * Optimisation - this is a deny everything to this user.
			 * Convert to an allow nothing and push to the end of the list.
			 */

			curr_ace->attr = ALLOW_ACE;
			curr_ace->perms = (mode_t)0;
			DLIST_DEMOTE(ace_list, curr_ace, canon_ace *);
			continue;
		}

		for (allow_ace_p = curr_ace->next; allow_ace_p; allow_ace_p = allow_ace_p->next) {

			if (allow_ace_p->attr != ALLOW_ACE)
				continue;

			/* We process GID_ACE and WORLD_ACE entries only. */

			if (allow_ace_p->owner_type == UID_ACE)
				continue;

			if (uid_entry_in_group( curr_ace, allow_ace_p))
				new_perms |= allow_ace_p->perms;
		}

		/*
		 * Convert to a allow entry, modify the perms and push to the end
		 * of the list.
		 */

		curr_ace->attr = ALLOW_ACE;
		curr_ace->perms = (new_perms & ~curr_ace->perms);
		DLIST_DEMOTE(ace_list, curr_ace, canon_ace *);
	}

	/* Pass 3 above - deal with deny group entries. */

	for (curr_ace = ace_list; curr_ace; curr_ace = curr_ace_next) {
		canon_ace *allow_ace_p;
		canon_ace *allow_everyone_p = NULL;

		curr_ace_next = curr_ace->next; /* So we can't lose the link. */

		if (curr_ace->attr != DENY_ACE)
			continue;

		if (curr_ace->owner_type != GID_ACE)
			continue;

		for (allow_ace_p = curr_ace->next; allow_ace_p; allow_ace_p = allow_ace_p->next) {

			if (allow_ace_p->attr != ALLOW_ACE)
				continue;

			/* Store a pointer to the Everyone allow, if it exists. */
			if (allow_ace_p->owner_type == WORLD_ACE)
				allow_everyone_p = allow_ace_p;

			/* We process UID_ACE entries only. */

			if (allow_ace_p->owner_type != UID_ACE)
				continue;

			/* Mask off the deny group perms. */

			if (uid_entry_in_group( allow_ace_p, curr_ace))
				allow_ace_p->perms &= ~curr_ace->perms;
		}

		/*
		 * Convert the deny to an allow with the correct perms and
		 * push to the end of the list.
		 */

		curr_ace->attr = ALLOW_ACE;
		if (allow_everyone_p)
			curr_ace->perms = allow_everyone_p->perms & ~curr_ace->perms;
		else
			curr_ace->perms = (mode_t)0;
		DLIST_DEMOTE(ace_list, curr_ace, canon_ace *);
	}

	/* Doing this fourth pass allows Windows semantics to be layered
	 * on top of POSIX semantics. I'm not sure if this is desirable.
	 * For example, in W2K ACLs there is no way to say, "Group X no
	 * access, user Y full access" if user Y is a member of group X.
	 * This seems completely broken semantics to me.... JRA.
	 */

#if 0
	/* Pass 4 above - deal with allow entries. */

	for (curr_ace = ace_list; curr_ace; curr_ace = curr_ace_next) {
		canon_ace *allow_ace_p;

		curr_ace_next = curr_ace->next; /* So we can't lose the link. */

		if (curr_ace->attr != ALLOW_ACE)
			continue;

		if (curr_ace->owner_type != UID_ACE)
			continue;

		for (allow_ace_p = ace_list; allow_ace_p; allow_ace_p = allow_ace_p->next) {

			if (allow_ace_p->attr != ALLOW_ACE)
				continue;

			/* We process GID_ACE entries only. */

			if (allow_ace_p->owner_type != GID_ACE)
				continue;

			/* OR in the group perms. */

			if (uid_entry_in_group( curr_ace, allow_ace_p))
				curr_ace->perms |= allow_ace_p->perms;
		}
	}
#endif

	*pp_ace_list = ace_list;
}