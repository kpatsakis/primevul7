R_API void *r_crbtree_take(RRBTree *tree, void *data, RRBComparator cmp, void *user) {
	r_return_val_if_fail (tree && data && tree->size && tree->root && cmp, NULL);

	RRBNode head; /* Fake tree root */
	memset (&head, 0, sizeof (RRBNode));
	RRBNode *q = &head, *p = NULL, *g = NULL;
	RRBNode *found = NULL;
	int dir = 1, last;

	_set_link (q, tree->root, 1);

	/* Find in-order predecessor */
	while (q->link[dir]) {
		last = dir;

		g = p;
		p = q;
		q = q->link[dir];

		dir = cmp (data, q->data, user);
		if (dir == 0 && !found) {
			found = q;
		}

		dir = (bool)(dir > 0);

		if (IS_RED (q) || IS_RED (q->link[dir])) {
			continue;
		}
		if (IS_RED (q->link[!dir])) {
			_set_link (p, _rot_once (q, dir), last);
			p = p->link[last];
		} else {
			RRBNode *sibling = p->link[!last];
			if (sibling) {
				if (!IS_RED (sibling->link[!last]) && !IS_RED (sibling->link[last])) {
					/* Color flip */
					p->red = 0;
					sibling->red = 1;
					q->red = 1;
				} else if (g) {
					int dir2 = (bool)(g->link[1] == p);

					if (IS_RED (sibling->link[last])) {
						_set_link (g, _rot_twice (p, last), dir2);
					} else {
						_set_link (g, _rot_once (p, last), dir2);
					}

					/* Ensure correct coloring */
					q->red = g->link[dir2]->red = 1;
					g->link[dir2]->link[0]->red = 0;
					g->link[dir2]->link[1]->red = 0;
				}
			}
		}
	}

	void *ret = NULL;
	/* Replace and remove if found */
	if (found) {
		_set_link (p, q->link[q->link[0] == NULL], p->link[1] == q);
		if (q != found) {
			q->link[0] = NULL;
			q->link[1] = NULL;
			q->parent = NULL;
			_exchange_nodes (found, q);
		}
		ret = found->data;
		free (found);
		tree->size--;
	}

	/* Update root node */
	tree->root = head.link[1];
	if (tree->root) {
		tree->root->red = 0;
		tree->root->parent = NULL;
	} else {
		r_return_val_if_fail (tree->size == 0, NULL);
	}
	return ret;
}