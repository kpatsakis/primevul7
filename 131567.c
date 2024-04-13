static void merge_aces( canon_ace **pp_list_head )
{
	canon_ace *list_head = *pp_list_head;
	canon_ace *curr_ace_outer;
	canon_ace *curr_ace_outer_next;

	/*
	 * First, merge allow entries with identical SIDs, and deny entries
	 * with identical SIDs.
	 */

	for (curr_ace_outer = list_head; curr_ace_outer; curr_ace_outer = curr_ace_outer_next) {
		canon_ace *curr_ace;
		canon_ace *curr_ace_next;

		curr_ace_outer_next = curr_ace_outer->next; /* Save the link in case we delete. */

		for (curr_ace = curr_ace_outer->next; curr_ace; curr_ace = curr_ace_next) {

			curr_ace_next = curr_ace->next; /* Save the link in case of delete. */

			if (identity_in_ace_equal(curr_ace, curr_ace_outer) &&
				(curr_ace->attr == curr_ace_outer->attr)) {

				if( DEBUGLVL( 10 )) {
					dbgtext("merge_aces: Merging ACE's\n");
					print_canon_ace( curr_ace_outer, 0);
					print_canon_ace( curr_ace, 0);
				}

				/* Merge two allow or two deny ACE's. */

				curr_ace_outer->perms |= curr_ace->perms;
				DLIST_REMOVE(list_head, curr_ace);
				SAFE_FREE(curr_ace);
				curr_ace_outer_next = curr_ace_outer->next; /* We may have deleted the link. */
			}
		}
	}

	/*
	 * Now go through and mask off allow permissions with deny permissions.
	 * We can delete either the allow or deny here as we know that each SID
	 * appears only once in the list.
	 */

	for (curr_ace_outer = list_head; curr_ace_outer; curr_ace_outer = curr_ace_outer_next) {
		canon_ace *curr_ace;
		canon_ace *curr_ace_next;

		curr_ace_outer_next = curr_ace_outer->next; /* Save the link in case we delete. */

		for (curr_ace = curr_ace_outer->next; curr_ace; curr_ace = curr_ace_next) {

			curr_ace_next = curr_ace->next; /* Save the link in case of delete. */

			/*
			 * Subtract ACE's with different entries. Due to the ordering constraints
			 * we've put on the ACL, we know the deny must be the first one.
			 */

			if (identity_in_ace_equal(curr_ace, curr_ace_outer) &&
				(curr_ace_outer->attr == DENY_ACE) && (curr_ace->attr == ALLOW_ACE)) {

				if( DEBUGLVL( 10 )) {
					dbgtext("merge_aces: Masking ACE's\n");
					print_canon_ace( curr_ace_outer, 0);
					print_canon_ace( curr_ace, 0);
				}

				curr_ace->perms &= ~curr_ace_outer->perms;

				if (curr_ace->perms == 0) {

					/*
					 * The deny overrides the allow. Remove the allow.
					 */

					DLIST_REMOVE(list_head, curr_ace);
					SAFE_FREE(curr_ace);
					curr_ace_outer_next = curr_ace_outer->next; /* We may have deleted the link. */

				} else {

					/*
					 * Even after removing permissions, there
					 * are still allow permissions - delete the deny.
					 * It is safe to delete the deny here,
					 * as we are guarenteed by the deny first
					 * ordering that all the deny entries for
					 * this SID have already been merged into one
					 * before we can get to an allow ace.
					 */

					DLIST_REMOVE(list_head, curr_ace_outer);
					SAFE_FREE(curr_ace_outer);
					break;
				}
			}

		} /* end for curr_ace */
	} /* end for curr_ace_outer */

	/* We may have modified the list. */

	*pp_list_head = list_head;
}