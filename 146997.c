R_API RRBNode *r_crbtree_first_node(RRBTree *tree) {
	r_return_val_if_fail (tree, NULL);
	if (!tree->root) {
		// empty tree
		return NULL;
	}
	RRBNode *node = tree->root;
	while (node->link[0]) {
		node = node->link[0];
	}
	return node;
}