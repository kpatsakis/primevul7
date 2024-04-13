R_API char *r_bin_java_print_methodtype_cp_stringify(RBinJavaCPTypeObj *obj) {
	return r_str_newf ("%d.0x%04"PFMT64x ".%s.%d",
			obj->metas->ord, obj->file_offset + obj->loadaddr, ((RBinJavaCPTypeMetas *) obj->metas->type_info)->name,
			obj->info.cp_method_type.descriptor_index);
}