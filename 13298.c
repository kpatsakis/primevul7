R_API void r_bin_java_get_import_json_definitions(RBinJavaObj *bin, PJ *pj) {
	r_return_if_fail (pj);
	RList *the_list;
	RListIter *iter = NULL;
	char *new_str;

	pj_ka (pj, "imports");

	if (!bin || !(the_list = r_bin_java_get_lib_names (bin))) {
		pj_end (pj);
		return;
	}

	r_list_foreach (the_list, iter, new_str) {
		char *tmp = new_str;
		while (*tmp) {
			if (*tmp == '/') {
				*tmp = '.';
			}
			tmp++;
		}
		pj_s (pj, new_str);
	}
	r_list_free (the_list);
	pj_end (pj);
	return;
}