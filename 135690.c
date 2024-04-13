int git_diff_basic_config(const char *var, const char *value, void *cb)
{
	if (!prefixcmp(var, "diff.color.") || !prefixcmp(var, "color.diff.")) {
		int slot = parse_diff_color_slot(var, 11);
		if (!value)
			return config_error_nonbool(var);
		color_parse(value, var, diff_colors[slot]);
		return 0;
	}

	if (!prefixcmp(var, "diff.")) {
		const char *ep = strrchr(var, '.');
		if (ep != var + 4) {
			if (!strcmp(ep, ".funcname")) {
				if (!value)
					return config_error_nonbool(var);
				return parse_funcname_pattern(var, ep, value);
			}
		}
	}

	return git_color_default_config(var, value, cb);
}