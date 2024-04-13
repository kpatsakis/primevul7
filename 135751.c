static int rev_same_tree_as_empty(struct rev_info *revs, struct tree *t1)
{
	int retval;
	void *tree;
	unsigned long size;
	struct tree_desc empty, real;

	if (!t1)
		return 0;

	tree = read_object_with_reference(t1->object.sha1, tree_type, &size, NULL);
	if (!tree)
		return 0;
	init_tree_desc(&real, tree, size);
	init_tree_desc(&empty, "", 0);

	tree_difference = REV_TREE_SAME;
	DIFF_OPT_CLR(&revs->pruning, HAS_CHANGES);
	retval = diff_tree(&empty, &real, "", &revs->pruning);
	free(tree);

	return retval >= 0 && (tree_difference == REV_TREE_SAME);
}