R_API void r_bin_java_print_name_and_type_cp_summary(RBinJavaCPTypeObj *obj) {
	if (!obj) {
		eprintf ("Attempting to print an invalid RBinJavaCPTypeObj*  Name_And_Type.\n");
		return;
	}
	printf ("Name_And_Type ConstantPool Type (%d) ", obj->metas->ord);
	printf ("  Offset: 0x%08"PFMT64x "", obj->file_offset);
	printf ("  name_idx = (%d)\n", obj->info.cp_name_and_type.name_idx);
	printf ("  descriptor_idx = (%d)\n", obj->info.cp_name_and_type.descriptor_idx);
}