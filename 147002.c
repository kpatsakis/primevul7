R_API RRBTree *r_crbtree_new(RRBFree freefn) {
	RRBTree *tree = R_NEW0 (RRBTree);
	if (tree) {
		tree->free = freefn;
	}
	return tree;
}