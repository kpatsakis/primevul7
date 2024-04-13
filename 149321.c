file_softmagic(struct magic_set *ms, const struct buffer *b,
    uint16_t *indir_count, uint16_t *name_count, int mode, int text)
{
	struct mlist *ml;
	int rv, printed_something = 0, need_separator = 0;
	uint16_t nc, ic;

	if (name_count == NULL) {
		nc = 0;
		name_count = &nc;
	}
	if (indir_count == NULL) {
		ic = 0;
		indir_count = &ic;
	}

	for (ml = ms->mlist[0]->next; ml != ms->mlist[0]; ml = ml->next)
		if ((rv = match(ms, ml->magic, ml->nmagic, b, 0, mode,
		    text, 0, indir_count, name_count,
		    &printed_something, &need_separator, NULL, NULL)) != 0)
			return rv;

	return 0;
}