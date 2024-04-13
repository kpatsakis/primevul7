R_API RList *r_bin_java_get_method_num_name(RBinJavaObj *bin_obj) {
	ut32 i = 0;
	RListIter *iter;
	RBinJavaField *fm_type;
	RList *res = r_list_newf (free);
	r_list_foreach (bin_obj->methods_list, iter, fm_type) {
		char *str = r_str_newf ("%d %s", i, fm_type->name);
		r_list_append (res, str);
		i++;
	}
	return res;
}