R_API char *r_bin_java_get_bin_obj_json(RBinJavaObj *bin) {
	PJ *pj = pj_new ();

	// this is a class dictionary
	pj_o (pj);

	// get the initial class dict data
	r_bin_java_get_class_info_json (bin, pj);

	// add named lists
	r_bin_java_get_method_json_definitions (bin, pj);
	r_bin_java_get_field_json_definitions (bin, pj);
	r_bin_java_get_import_json_definitions (bin, pj);
	//r_bin_java_get_interface_json_definitions (bin, pj);

	pj_end (pj);

	return pj_drain (pj);
}