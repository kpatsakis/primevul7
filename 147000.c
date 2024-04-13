R_API void *r_crbtree_find(RRBTree *tree, void *data, RRBComparator cmp, void *user) {
	r_return_val_if_fail (tree && cmp, NULL);
	RRBNode *node = r_crbtree_find_node (tree, data, cmp, user);
	return node ? node->data : NULL;
}