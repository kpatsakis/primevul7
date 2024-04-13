static void checkdiff_consume(void *priv, char *line, unsigned long len)
{
	struct checkdiff_t *data = priv;
	const char *ws = diff_get_color(data->color_diff, DIFF_WHITESPACE);
	const char *reset = diff_get_color(data->color_diff, DIFF_RESET);
	const char *set = diff_get_color(data->color_diff, DIFF_FILE_NEW);
	char *err;

	if (line[0] == '+') {
		unsigned bad;
		data->lineno++;
		bad = check_and_emit_line(line + 1, len - 1,
		    data->ws_rule, NULL, NULL, NULL, NULL);
		if (!bad)
			return;
		data->status |= bad;
		err = whitespace_error_string(bad);
		fprintf(data->file, "%s:%d: %s.\n", data->filename, data->lineno, err);
		free(err);
		emit_line(data->file, set, reset, line, 1);
		(void)check_and_emit_line(line + 1, len - 1, data->ws_rule,
		    data->file, set, reset, ws);
	} else if (line[0] == ' ')
		data->lineno++;
	else if (line[0] == '@') {
		char *plus = strchr(line, '+');
		if (plus)
			data->lineno = strtol(plus, NULL, 10) - 1;
		else
			die("invalid diff");
	}
}