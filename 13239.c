R_API char *r_bin_java_print_null_cp_stringify(RBinJavaCPTypeObj *obj) {
	return r_str_newf ("%d.0x%04"PFMT64x ".%s",
		obj->metas->ord,
		obj->file_offset + obj->loadaddr,
		((RBinJavaCPTypeMetas *) obj->metas->type_info)->name);
}