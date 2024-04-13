R_API RRBNode *r_rbnode_next(RRBNode *node) {
	r_return_val_if_fail (node, NULL);
	if (node->link[1]) {
		node = node->link[1];
		while (node->link[0]) {
			node = node->link[0];
		}
		return node;
	}
	RRBNode *parent = node->parent;
	while (parent && parent->link[1] == node) {
		node = parent;
		parent = node->parent;
	}
	return parent;
}