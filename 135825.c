enum commit_action simplify_commit(struct rev_info *revs, struct commit *commit)
{
	if (commit->object.flags & SHOWN)
		return commit_ignore;
	if (revs->unpacked && has_sha1_pack(commit->object.sha1, revs->ignore_packed))
		return commit_ignore;
	if (revs->show_all)
		return commit_show;
	if (commit->object.flags & UNINTERESTING)
		return commit_ignore;
	if (revs->min_age != -1 && (commit->date > revs->min_age))
		return commit_ignore;
	if (revs->no_merges && commit->parents && commit->parents->next)
		return commit_ignore;
	if (!commit_match(commit, revs))
		return commit_ignore;
	if (revs->prune && revs->dense) {
		/* Commit without changes? */
		if (commit->object.flags & TREESAME) {
			/* drop merges unless we want parenthood */
			if (!revs->rewrite_parents)
				return commit_ignore;
			/* non-merge - always ignore it */
			if (!commit->parents || !commit->parents->next)
				return commit_ignore;
		}
		if (revs->rewrite_parents && rewrite_parents(revs, commit) < 0)
			return commit_error;
	}
	return commit_show;
}