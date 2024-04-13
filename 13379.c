R_API void r_bin_java_print_methodtype_cp_summary(RBinJavaCPTypeObj *obj) {
	if (!obj) {
		eprintf ("Attempting to print an invalid RBinJavaCPTypeObj*  RBinJavaCPTypeMethodType.\n");
		return;
	}
	printf ("MethodType ConstantPool Type (%d) ", obj->metas->ord);
	printf ("  Offset: 0x%08"PFMT64x "", obj->file_offset);
	printf ("  Descriptor Index = 0x%02x\n", obj->info.cp_method_type.descriptor_index);
}