R_API RRBNode *r_rbnode_prev(RRBNode *node) {
	r_return_val_if_fail (node, NULL);
	if (node->link[0]) {
		node = node->link[0];
		while (node->link[1]) {
			node = node->link[1];
		}
		return node;
	}
	RRBNode *parent = node->parent;
	while (parent && parent->link[0] == node) {
		node = parent;
		parent = node->parent;
	}
	return parent;
}