static int rev_compare_tree(struct rev_info *revs, struct tree *t1, struct tree *t2)
{
	if (!t1)
		return REV_TREE_NEW;
	if (!t2)
		return REV_TREE_DIFFERENT;
	tree_difference = REV_TREE_SAME;
	DIFF_OPT_CLR(&revs->pruning, HAS_CHANGES);
	if (diff_tree_sha1(t1->object.sha1, t2->object.sha1, "",
			   &revs->pruning) < 0)
		return REV_TREE_DIFFERENT;
	return tree_difference;
}