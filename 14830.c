void sort_directory(struct dir *dir)
{
	struct dir_ent *cur, *l1, *l2, *next;
	int len1, len2, stride = 1;

	if(dir->dir_count < 2)
		return;

	/*
	 * We can consider our linked-list to be made up of stride length
	 * sublists.  Eacn iteration around this loop merges adjacent
	 * stride length sublists into larger 2*stride sublists.  We stop
	 * when stride becomes equal to the entire list.
	 *
	 * Initially stride = 1 (by definition a sublist of 1 is sorted), and
	 * these 1 element sublists are merged into 2 element sublists,  which
	 * are then merged into 4 element sublists and so on.
	 */
	do {
		l2 = dir->dirs; /* head of current linked list */
		cur = NULL; /* empty output list */

		/*
		 * Iterate through the linked list, merging adjacent sublists.
		 * On each interation l2 points to the next sublist pair to be
		 * merged (if there's only one sublist left this is simply added
		 * to the output list)
		 */
		while(l2) {
			l1 = l2;
			for(len1 = 0; l2 && len1 < stride; len1 ++, l2 = l2->next);
			len2 = stride;

			/*
			 * l1 points to first sublist.
			 * l2 points to second sublist.
			 * Merge them onto the output list
			 */
			while(len1 && l2 && len2) {
				if(strcmp(l1->name, l2->name) <= 0) {
					next = l1;
					l1 = l1->next;
					len1 --;
				} else {
					next = l2;
					l2 = l2->next;
					len2 --;
				}

				if(cur) {
					cur->next = next;
					cur = next;
				} else
					dir->dirs = cur = next;
			}
			/*
			 * One sublist is now empty, copy the other one onto the
			 * output list
			 */
			for(; len1; len1 --, l1 = l1->next) {
				if(cur) {
					cur->next = l1;
					cur = l1;
				} else
					dir->dirs = cur = l1;
			}
			for(; l2 && len2; len2 --, l2 = l2->next) {
				if(cur) {
					cur->next = l2;
					cur = l2;
				} else
					dir->dirs = cur = l2;
			}
		}
		cur->next = NULL;
		stride = stride << 1;
	} while(stride < dir->dir_count);
}