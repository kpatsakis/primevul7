static void _set_link(RRBNode *parent, RRBNode *child, const int dir) {
	if (parent) {
		parent->link[dir] = child;
	}
	if (child) {
		child->parent = parent;
	}
}