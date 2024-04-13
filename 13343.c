R_API void r_bin_java_print_verification_info_summary(RBinJavaVerificationObj *obj) {
	ut8 tag_value = R_BIN_JAVA_STACKMAP_UNKNOWN;
	if (!obj) {
		eprintf ("Attempting to print an invalid RBinJavaVerificationObj*  .\n");
		return;
	}
	if (obj->tag < R_BIN_JAVA_STACKMAP_UNKNOWN) {
		tag_value = obj->tag;
	}
	printf ("Verification Information\n");
	printf ("  Offset: 0x%08"PFMT64x "", obj->file_offset);
	printf ("  Tag Value = 0x%02x\n", obj->tag);
	printf ("  Name = %s\n", R_BIN_JAVA_VERIFICATION_METAS[tag_value].name);
	if (obj->tag == R_BIN_JAVA_STACKMAP_OBJECT) {
		printf ("  Object Constant Pool Index = 0x%x\n", obj->info.obj_val_cp_idx);
	} else if (obj->tag == R_BIN_JAVA_STACKMAP_UNINIT) {
		printf ("  Uninitialized Object offset in code = 0x%x\n", obj->info.uninit_offset);
	}
}