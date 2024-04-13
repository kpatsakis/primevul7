R_API char *r_bin_java_build_obj_key(RBinJavaObj *bin) {
	char *cname = r_bin_java_get_this_class_name (bin);
	char *jvcname = cname?
		r_str_newf ("%d.%s.class", bin->id, cname)
		: r_str_newf ("%d._unknown_.class", bin->id);
	free (cname);
	return jvcname;
}