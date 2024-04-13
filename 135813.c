static void run_checkdiff(struct diff_filepair *p, struct diff_options *o)
{
	const char *name;
	const char *other;
	const char *attr_path;

	if (DIFF_PAIR_UNMERGED(p)) {
		/* unmerged */
		return;
	}

	name = p->one->path;
	other = (strcmp(name, p->two->path) ? p->two->path : NULL);
	attr_path = other ? other : name;

	if (o->prefix_length)
		strip_prefix(o->prefix_length, &name, &other);

	diff_fill_sha1_info(p->one);
	diff_fill_sha1_info(p->two);

	builtin_checkdiff(name, other, attr_path, p->one, p->two, o);
}