static void fn_out_consume(void *priv, char *line, unsigned long len)
{
	int i;
	int color;
	struct emit_callback *ecbdata = priv;
	const char *meta = diff_get_color(ecbdata->color_diff, DIFF_METAINFO);
	const char *plain = diff_get_color(ecbdata->color_diff, DIFF_PLAIN);
	const char *reset = diff_get_color(ecbdata->color_diff, DIFF_RESET);

	*(ecbdata->found_changesp) = 1;

	if (ecbdata->label_path[0]) {
		const char *name_a_tab, *name_b_tab;

		name_a_tab = strchr(ecbdata->label_path[0], ' ') ? "\t" : "";
		name_b_tab = strchr(ecbdata->label_path[1], ' ') ? "\t" : "";

		fprintf(ecbdata->file, "%s--- %s%s%s\n",
			meta, ecbdata->label_path[0], reset, name_a_tab);
		fprintf(ecbdata->file, "%s+++ %s%s%s\n",
			meta, ecbdata->label_path[1], reset, name_b_tab);
		ecbdata->label_path[0] = ecbdata->label_path[1] = NULL;
	}

	/* This is not really necessary for now because
	 * this codepath only deals with two-way diffs.
	 */
	for (i = 0; i < len && line[i] == '@'; i++)
		;
	if (2 <= i && i < len && line[i] == ' ') {
		ecbdata->nparents = i - 1;
		len = sane_truncate_line(ecbdata, line, len);
		emit_line(ecbdata->file,
			  diff_get_color(ecbdata->color_diff, DIFF_FRAGINFO),
			  reset, line, len);
		if (line[len-1] != '\n')
			putc('\n', ecbdata->file);
		return;
	}

	if (len < ecbdata->nparents) {
		emit_line(ecbdata->file, reset, reset, line, len);
		return;
	}

	color = DIFF_PLAIN;
	if (ecbdata->diff_words && ecbdata->nparents != 1)
		/* fall back to normal diff */
		free_diff_words_data(ecbdata);
	if (ecbdata->diff_words) {
		if (line[0] == '-') {
			diff_words_append(line, len,
					  &ecbdata->diff_words->minus);
			return;
		} else if (line[0] == '+') {
			diff_words_append(line, len,
					  &ecbdata->diff_words->plus);
			return;
		}
		if (ecbdata->diff_words->minus.text.size ||
		    ecbdata->diff_words->plus.text.size)
			diff_words_show(ecbdata->diff_words);
		line++;
		len--;
		emit_line(ecbdata->file, plain, reset, line, len);
		return;
	}
	for (i = 0; i < ecbdata->nparents && len; i++) {
		if (line[i] == '-')
			color = DIFF_FILE_OLD;
		else if (line[i] == '+')
			color = DIFF_FILE_NEW;
	}

	if (color != DIFF_FILE_NEW) {
		emit_line(ecbdata->file,
			  diff_get_color(ecbdata->color_diff, color),
			  reset, line, len);
		return;
	}
	emit_add_line(reset, ecbdata, line, len);
}