R_API void r_crbtree_free(RRBTree *tree) {
	if (!tree) {
		return;
	}
	r_crbtree_clear (tree);
	free (tree);
}