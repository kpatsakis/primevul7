static void _exchange_nodes(RRBNode *node_a, RRBNode *node_b) {
	if (!node_a || !node_b) {
		return;
	}
	RRBNode node_a_tmp, node_b_tmp;
	memcpy (&node_a_tmp, node_a, sizeof (RRBNode));
	memcpy (&node_b_tmp, node_b, sizeof (RRBNode));
	node_a->link[0] = node_b_tmp.link[0];
	node_a->link[1] = node_b_tmp.link[1];
	node_a->red = node_b_tmp.red;
	node_b->link[0] = node_a_tmp.link[0];
	node_b->link[1] = node_a_tmp.link[1];
	node_b->red = node_a_tmp.red;
	if (node_a->parent == node_b->parent) {
		if (node_a->parent) {
			if (node_a->parent->link[0] == node_a) {
				node_a->parent->link[0] = node_b;
				node_a->parent->link[1] = node_a;
			} else {
				node_a->parent->link[1] = node_b;
				node_a->parent->link[0] = node_a;
			}
		}
		if (node_a->link[0]) {
			node_a->link[0]->parent = node_a;
		}
		if (node_a->link[1]) {
			node_a->link[1]->parent = node_a;
		}
		if (node_b->link[0]) {
			node_b->link[0]->parent = node_b;
		}
		if (node_b->link[1]) {
			node_b->link[1]->parent = node_b;
		}
		return;
	}
	RRBNode *parent_a = node_a->parent;
	RRBNode *parent_b = node_b->parent;
	if (parent_a) {
		if (parent_a->link[0] == node_a) {
			parent_a->link[0] = node_b;
		} else {
			parent_a->link[1] = node_b;
		}
	}
	node_b->parent = parent_a;
	if (parent_b) {
		if (parent_b->link[0] == node_b) {
			parent_b->link[0] = node_a;
		} else {
			parent_b->link[1] = node_a;
		}
	}
	node_a->parent = parent_b;
	if (node_a->link[0]) {
		node_a->link[0]->parent = node_a;
	}
	if (node_a->link[1]) {
		node_a->link[1]->parent = node_a;
	}
	if (node_b->link[0]) {
		node_b->link[0]->parent = node_b;
	}
	if (node_b->link[1]) {
		node_b->link[1]->parent = node_b;
	}
}