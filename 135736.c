int handle_revision_arg(const char *arg, struct rev_info *revs,
			int flags,
			int cant_be_filename)
{
	unsigned mode;
	char *dotdot;
	struct object *object;
	unsigned char sha1[20];
	int local_flags;

	dotdot = strstr(arg, "..");
	if (dotdot) {
		unsigned char from_sha1[20];
		const char *next = dotdot + 2;
		const char *this = arg;
		int symmetric = *next == '.';
		unsigned int flags_exclude = flags ^ UNINTERESTING;

		*dotdot = 0;
		next += symmetric;

		if (!*next)
			next = "HEAD";
		if (dotdot == arg)
			this = "HEAD";
		if (!get_sha1(this, from_sha1) &&
		    !get_sha1(next, sha1)) {
			struct commit *a, *b;
			struct commit_list *exclude;

			a = lookup_commit_reference(from_sha1);
			b = lookup_commit_reference(sha1);
			if (!a || !b) {
				die(symmetric ?
				    "Invalid symmetric difference expression %s...%s" :
				    "Invalid revision range %s..%s",
				    arg, next);
			}

			if (!cant_be_filename) {
				*dotdot = '.';
				verify_non_filename(revs->prefix, arg);
			}

			if (symmetric) {
				exclude = get_merge_bases(a, b, 1);
				add_pending_commit_list(revs, exclude,
							flags_exclude);
				free_commit_list(exclude);
				a->object.flags |= flags | SYMMETRIC_LEFT;
			} else
				a->object.flags |= flags_exclude;
			b->object.flags |= flags;
			add_pending_object(revs, &a->object, this);
			add_pending_object(revs, &b->object, next);
			return 0;
		}
		*dotdot = '.';
	}
	dotdot = strstr(arg, "^@");
	if (dotdot && !dotdot[2]) {
		*dotdot = 0;
		if (add_parents_only(revs, arg, flags))
			return 0;
		*dotdot = '^';
	}
	dotdot = strstr(arg, "^!");
	if (dotdot && !dotdot[2]) {
		*dotdot = 0;
		if (!add_parents_only(revs, arg, flags ^ UNINTERESTING))
			*dotdot = '^';
	}

	local_flags = 0;
	if (*arg == '^') {
		local_flags = UNINTERESTING;
		arg++;
	}
	if (get_sha1_with_mode(arg, sha1, &mode))
		return -1;
	if (!cant_be_filename)
		verify_non_filename(revs->prefix, arg);
	object = get_reference(revs, arg, sha1, flags ^ local_flags);
	add_pending_object_with_mode(revs, object, arg, mode);
	return 0;
}