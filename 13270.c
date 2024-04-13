R_API void r_bin_java_get_fm_type_definition_json(RBinJavaObj *bin, RBinJavaField *fm_type, PJ *pj, int is_method) {
	r_return_if_fail (bin && fm_type && pj);

	ut64 addr = UT64_MAX;
	char *prototype = NULL, *fq_name = NULL;
	bool is_native = ((fm_type->flags & R_BIN_JAVA_METHOD_ACC_NATIVE) != 0);
	bool is_static = ((fm_type->flags & R_BIN_JAVA_METHOD_ACC_STATIC) != 0);
	bool is_synthetic = ((fm_type->flags & R_BIN_JAVA_METHOD_ACC_SYNTHETIC) != 0);
	bool is_private = ((fm_type->flags & R_BIN_JAVA_METHOD_ACC_PRIVATE) != 0);
	bool is_public = ((fm_type->flags & R_BIN_JAVA_METHOD_ACC_PUBLIC) != 0);
	bool is_protected = ((fm_type->flags & R_BIN_JAVA_METHOD_ACC_PROTECTED) != 0);
	bool is_super = ((fm_type->flags & R_BIN_JAVA_CLASS_ACC_SUPER) != 0);

	pj_o (pj);
	pj_ki (pj, "access_flags", fm_type->flags);
	pj_ki (pj, "is_method", is_method);
	pj_ki (pj, "is_native", is_native);
	pj_ki (pj, "is_synthetic", is_synthetic);
	pj_ki (pj, "is_private", is_private);
	pj_ki (pj, "is_public", is_public);
	pj_ki (pj, "is_static", is_static);
	pj_ki (pj, "is_protected", is_protected);
	pj_ki (pj, "is_super", is_super);

	addr = r_bin_java_get_method_code_offset (fm_type);
	if (addr == 0) {
		addr = fm_type->file_offset;
	}
	addr += bin->loadaddr;

	pj_ki (pj, "addr", addr);
	pj_ki (pj, "offset", fm_type->file_offset + bin->loadaddr);
	pj_ks (pj, "class_name", fm_type->class_name);
	pj_ks (pj, "signature", fm_type->descriptor);
	pj_ks (pj, "name", fm_type->name);

	if (is_method) {
		fq_name = r_bin_java_create_method_fq_str (fm_type->class_name, fm_type->name, fm_type->descriptor);
	} else {
		fq_name = r_bin_java_create_field_fq_str (fm_type->class_name, fm_type->name, fm_type->descriptor);
	}
	pj_ks (pj, "fq_name", fq_name);
	free (fq_name);

	prototype = r_bin_java_unmangle (fm_type->flags_str, fm_type->name, fm_type->descriptor);
	pj_ks (pj, "prototype", prototype);
	free (prototype);

	pj_end (pj);
}