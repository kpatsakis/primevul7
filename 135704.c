static struct commit *get_revision_internal(struct rev_info *revs)
{
	struct commit *c = NULL;
	struct commit_list *l;

	if (revs->boundary == 2) {
		/*
		 * All of the normal commits have already been returned,
		 * and we are now returning boundary commits.
		 * create_boundary_commit_list() has populated
		 * revs->commits with the remaining commits to return.
		 */
		c = pop_commit(&revs->commits);
		if (c)
			c->object.flags |= SHOWN;
		return c;
	}

	if (revs->reverse) {
		int limit = -1;

		if (0 <= revs->max_count) {
			limit = revs->max_count;
			if (0 < revs->skip_count)
				limit += revs->skip_count;
		}
		l = NULL;
		while ((c = get_revision_1(revs))) {
			commit_list_insert(c, &l);
			if ((0 < limit) && !--limit)
				break;
		}
		revs->commits = l;
		revs->reverse = 0;
		revs->max_count = -1;
		c = NULL;
	}

	/*
	 * Now pick up what they want to give us
	 */
	c = get_revision_1(revs);
	if (c) {
		while (0 < revs->skip_count) {
			revs->skip_count--;
			c = get_revision_1(revs);
			if (!c)
				break;
		}
	}

	/*
	 * Check the max_count.
	 */
	switch (revs->max_count) {
	case -1:
		break;
	case 0:
		c = NULL;
		break;
	default:
		revs->max_count--;
	}

	if (c)
		c->object.flags |= SHOWN;

	if (!revs->boundary) {
		return c;
	}

	if (!c) {
		/*
		 * get_revision_1() runs out the commits, and
		 * we are done computing the boundaries.
		 * switch to boundary commits output mode.
		 */
		revs->boundary = 2;

		/*
		 * Update revs->commits to contain the list of
		 * boundary commits.
		 */
		create_boundary_commit_list(revs);

		return get_revision_internal(revs);
	}

	/*
	 * boundary commits are the commits that are parents of the
	 * ones we got from get_revision_1() but they themselves are
	 * not returned from get_revision_1().  Before returning
	 * 'c', we need to mark its parents that they could be boundaries.
	 */

	for (l = c->parents; l; l = l->next) {
		struct object *p;
		p = &(l->item->object);
		if (p->flags & (CHILD_SHOWN | SHOWN))
			continue;
		p->flags |= CHILD_SHOWN;
		gc_boundary(&revs->boundary_commits);
		add_object_array(p, NULL, &revs->boundary_commits);
	}

	return c;
}