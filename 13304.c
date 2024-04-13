R_API void r_bin_java_get_field_json_definitions(RBinJavaObj *bin, PJ *pj) {
	r_return_if_fail (pj);
	RBinJavaField *fm_type = NULL;
	RListIter *iter = NULL;

	pj_ka (pj, "fields");
	if (!bin) {
		pj_end (pj);
		return;
	}

	r_list_foreach (bin->fields_list, iter, fm_type) {
		r_bin_java_get_field_json_definition (bin, fm_type, pj);
	}
	pj_end (pj);
}