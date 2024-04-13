R_API char *r_bin_java_print_methodhandle_cp_stringify(RBinJavaCPTypeObj *obj) {
	ut8 ref_kind = obj->info.cp_method_handle.reference_kind;
	return r_str_newf ("%d.0x%04"PFMT64x ".%s.%s.%d",
			obj->metas->ord, obj->file_offset + obj->loadaddr, ((RBinJavaCPTypeMetas *) obj->metas->type_info)->name,
			R_BIN_JAVA_REF_METAS[ref_kind].name, obj->info.cp_method_handle.reference_index);
}