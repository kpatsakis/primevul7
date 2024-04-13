R_API void r_crbtree_clear(RRBTree *tree) {
	r_return_if_fail (tree);
	RRBNode *iter = tree->root, *save = NULL;

	// Rotate away the left links into a linked list so that
	// we can perform iterative destruction of the rbtree
	while (iter) {
		if (!iter->link[0]) {
			save = iter->link[1];
			if (tree->free) {
				tree->free (iter->data);
			}
			free (iter);
			tree->size--;
		} else {
			save = iter->link[0];
			_set_link (iter, save->link[1], 0);
			_set_link (save, iter, 1);
		}
		iter = save;
	}
	tree->root = NULL;
}