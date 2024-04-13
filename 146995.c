R_API bool r_crbtree_delete(RRBTree *tree, void *data, RRBComparator cmp, void *user) {
	r_return_val_if_fail (tree && data && tree->size && tree->root && cmp, false);
	data = r_crbtree_take (tree, data, cmp, user);
	if (tree->free) {
		tree->free (data);
	}
	return !!data;
}