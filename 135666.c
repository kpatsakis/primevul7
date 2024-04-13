static void run_diff_cmd(const char *pgm,
			 const char *name,
			 const char *other,
			 const char *attr_path,
			 struct diff_filespec *one,
			 struct diff_filespec *two,
			 const char *xfrm_msg,
			 struct diff_options *o,
			 int complete_rewrite)
{
	if (!DIFF_OPT_TST(o, ALLOW_EXTERNAL))
		pgm = NULL;
	else {
		const char *cmd = external_diff_attr(attr_path);
		if (cmd)
			pgm = cmd;
	}

	if (pgm) {
		run_external_diff(pgm, name, other, one, two, xfrm_msg,
				  complete_rewrite);
		return;
	}
	if (one && two)
		builtin_diff(name, other ? other : name,
			     one, two, xfrm_msg, o, complete_rewrite);
	else
		fprintf(o->file, "* Unmerged path %s\n", name);
}