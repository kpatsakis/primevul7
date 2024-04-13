R_API RRBNode *r_crbtree_find_node(RRBTree *tree, void *data, RRBComparator cmp, void *user) {
	r_return_val_if_fail (tree && cmp, NULL);

	RRBNode *iter = tree->root;
	while (iter) {
		const int dir = cmp (data, iter->data, user);
		if (!dir) {
			return iter;
		}
		iter = iter->link[dir > 0];
	}
	return NULL;
}