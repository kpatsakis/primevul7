int git_diff_ui_config(const char *var, const char *value, void *cb)
{
	if (!strcmp(var, "diff.renamelimit")) {
		diff_rename_limit_default = git_config_int(var, value);
		return 0;
	}
	if (!strcmp(var, "diff.color") || !strcmp(var, "color.diff")) {
		diff_use_color_default = git_config_colorbool(var, value, -1);
		return 0;
	}
	if (!strcmp(var, "diff.renames")) {
		if (!value)
			diff_detect_rename_default = DIFF_DETECT_RENAME;
		else if (!strcasecmp(value, "copies") ||
			 !strcasecmp(value, "copy"))
			diff_detect_rename_default = DIFF_DETECT_COPY;
		else if (git_config_bool(var,value))
			diff_detect_rename_default = DIFF_DETECT_RENAME;
		return 0;
	}
	if (!strcmp(var, "diff.autorefreshindex")) {
		diff_auto_refresh_index = git_config_bool(var, value);
		return 0;
	}
	if (!strcmp(var, "diff.external"))
		return git_config_string(&external_diff_cmd_cfg, var, value);
	if (!prefixcmp(var, "diff.")) {
		const char *ep = strrchr(var, '.');

		if (ep != var + 4 && !strcmp(ep, ".command"))
			return parse_lldiff_command(var, ep, value);
	}

	return git_diff_basic_config(var, value, cb);
}