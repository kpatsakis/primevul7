R_API void r_bin_java_print_string_cp_summary(RBinJavaCPTypeObj *obj) {
	if (!obj) {
		eprintf ("Attempting to print an invalid RBinJavaCPTypeObj*  String.\n");
		return;
	}
	printf ("String ConstantPool Type (%d) ", obj->metas->ord);
	printf ("  Offset: 0x%08"PFMT64x "", obj->file_offset);
	printf ("  String Index = %d\n", obj->info.cp_string.string_idx);
}