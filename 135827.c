static int parse_diff_color_slot(const char *var, int ofs)
{
	if (!strcasecmp(var+ofs, "plain"))
		return DIFF_PLAIN;
	if (!strcasecmp(var+ofs, "meta"))
		return DIFF_METAINFO;
	if (!strcasecmp(var+ofs, "frag"))
		return DIFF_FRAGINFO;
	if (!strcasecmp(var+ofs, "old"))
		return DIFF_FILE_OLD;
	if (!strcasecmp(var+ofs, "new"))
		return DIFF_FILE_NEW;
	if (!strcasecmp(var+ofs, "commit"))
		return DIFF_COMMIT;
	if (!strcasecmp(var+ofs, "whitespace"))
		return DIFF_WHITESPACE;
	die("bad config variable '%s'", var);
}