int diff_root_tree_sha1(const unsigned char *new, const char *base, struct diff_options *opt)
{
	int retval;
	void *tree;
	unsigned long size;
	struct tree_desc empty, real;

	tree = read_object_with_reference(new, tree_type, &size, NULL);
	if (!tree)
		die("unable to read root tree (%s)", sha1_to_hex(new));
	init_tree_desc(&real, tree, size);

	init_tree_desc(&empty, "", 0);
	retval = diff_tree(&empty, &real, base, opt);
	free(tree);
	return retval;
}