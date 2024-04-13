static void diffcore_skip_stat_unmatch(struct diff_options *diffopt)
{
	int i;
	struct diff_queue_struct *q = &diff_queued_diff;
	struct diff_queue_struct outq;
	outq.queue = NULL;
	outq.nr = outq.alloc = 0;

	for (i = 0; i < q->nr; i++) {
		struct diff_filepair *p = q->queue[i];

		/*
		 * 1. Entries that come from stat info dirtyness
		 *    always have both sides (iow, not create/delete),
		 *    one side of the object name is unknown, with
		 *    the same mode and size.  Keep the ones that
		 *    do not match these criteria.  They have real
		 *    differences.
		 *
		 * 2. At this point, the file is known to be modified,
		 *    with the same mode and size, and the object
		 *    name of one side is unknown.  Need to inspect
		 *    the identical contents.
		 */
		if (!DIFF_FILE_VALID(p->one) || /* (1) */
		    !DIFF_FILE_VALID(p->two) ||
		    (p->one->sha1_valid && p->two->sha1_valid) ||
		    (p->one->mode != p->two->mode) ||
		    diff_populate_filespec(p->one, 1) ||
		    diff_populate_filespec(p->two, 1) ||
		    (p->one->size != p->two->size) ||
		    !diff_filespec_is_identical(p->one, p->two)) /* (2) */
			diff_q(&outq, p);
		else {
			/*
			 * The caller can subtract 1 from skip_stat_unmatch
			 * to determine how many paths were dirty only
			 * due to stat info mismatch.
			 */
			if (!DIFF_OPT_TST(diffopt, NO_INDEX))
				diffopt->skip_stat_unmatch++;
			diff_free_filepair(p);
		}
	}
	free(q->queue);
	*q = outq;
}