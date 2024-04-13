static RRBNode *_node_new(void *data, RRBNode *parent) {
	RRBNode *node = R_NEW0 (RRBNode);
	r_return_val_if_fail (node, NULL);

	node->red = 1;
	node->data = data;
	node->parent = parent;

	return node;
}