R_API RRBNode *r_crbtree_last_node(RRBTree *tree) {
	r_return_val_if_fail (tree, NULL);
	if (!tree->root) {
		// empty tree
		return NULL;
	}
	RRBNode *node = tree->root;
	while (node->link[1]) {
		node = node->link[1];
	}
	return node;
}