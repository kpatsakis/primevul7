static void emit_rewrite_diff(const char *name_a,
			      const char *name_b,
			      struct diff_filespec *one,
			      struct diff_filespec *two,
			      struct diff_options *o)
{
	int lc_a, lc_b;
	int color_diff = DIFF_OPT_TST(o, COLOR_DIFF);
	const char *name_a_tab, *name_b_tab;
	const char *metainfo = diff_get_color(color_diff, DIFF_METAINFO);
	const char *fraginfo = diff_get_color(color_diff, DIFF_FRAGINFO);
	const char *old = diff_get_color(color_diff, DIFF_FILE_OLD);
	const char *new = diff_get_color(color_diff, DIFF_FILE_NEW);
	const char *reset = diff_get_color(color_diff, DIFF_RESET);
	static struct strbuf a_name = STRBUF_INIT, b_name = STRBUF_INIT;

	name_a += (*name_a == '/');
	name_b += (*name_b == '/');
	name_a_tab = strchr(name_a, ' ') ? "\t" : "";
	name_b_tab = strchr(name_b, ' ') ? "\t" : "";

	strbuf_reset(&a_name);
	strbuf_reset(&b_name);
	quote_two_c_style(&a_name, o->a_prefix, name_a, 0);
	quote_two_c_style(&b_name, o->b_prefix, name_b, 0);

	diff_populate_filespec(one, 0);
	diff_populate_filespec(two, 0);
	lc_a = count_lines(one->data, one->size);
	lc_b = count_lines(two->data, two->size);
	fprintf(o->file,
		"%s--- %s%s%s\n%s+++ %s%s%s\n%s@@ -",
		metainfo, a_name.buf, name_a_tab, reset,
		metainfo, b_name.buf, name_b_tab, reset, fraginfo);
	print_line_count(o->file, lc_a);
	fprintf(o->file, " +");
	print_line_count(o->file, lc_b);
	fprintf(o->file, " @@%s\n", reset);
	if (lc_a)
		copy_file_with_prefix(o->file, '-', one->data, one->size, old, reset);
	if (lc_b)
		copy_file_with_prefix(o->file, '+', two->data, two->size, new, reset);
}