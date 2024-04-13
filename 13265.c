R_API void r_bin_java_print_long_cp_summary(RBinJavaCPTypeObj *obj) {
	ut8 *b = NULL;
	if (!obj) {
		eprintf ("Attempting to print an invalid RBinJavaCPTypeObj*  Long.\n");
		return;
	}
	b = obj->info.cp_long.bytes.raw;
	printf ("Long ConstantPool Type (%d) ", obj->metas->ord);
	printf ("  Offset: 0x%08"PFMT64x "", obj->file_offset);
	printf ("  High-Bytes = %02x %02x %02x %02x\n", b[0], b[1], b[2], b[3]);
	printf ("  Low-Bytes = %02x %02x %02x %02x\n", b[4], b[5], b[6], b[7]);
	printf ("  Long = %08"PFMT64x "\n", r_bin_java_raw_to_long (obj->info.cp_long.bytes.raw, 0));
}