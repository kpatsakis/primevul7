static void add_ignore_packed(struct rev_info *revs, const char *name)
{
	int num = ++revs->num_ignore_packed;

	revs->ignore_packed = xrealloc(revs->ignore_packed,
				       sizeof(const char **) * (num + 1));
	revs->ignore_packed[num-1] = name;
	revs->ignore_packed[num] = NULL;
}