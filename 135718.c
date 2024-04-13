static void emit_add_line(const char *reset, struct emit_callback *ecbdata, const char *line, int len)
{
	const char *ws = diff_get_color(ecbdata->color_diff, DIFF_WHITESPACE);
	const char *set = diff_get_color(ecbdata->color_diff, DIFF_FILE_NEW);

	if (!*ws)
		emit_line(ecbdata->file, set, reset, line, len);
	else {
		/* Emit just the prefix, then the rest. */
		emit_line(ecbdata->file, set, reset, line, ecbdata->nparents);
		(void)check_and_emit_line(line + ecbdata->nparents,
		    len - ecbdata->nparents, ecbdata->ws_rule,
		    ecbdata->file, set, reset, ws);
	}
}