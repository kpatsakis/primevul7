R_API void r_bin_java_print_source_debug_attr_summary(RBinJavaAttrInfo *attr) {
	ut32 i = 0;
	if (attr == NULL) {
		eprintf ("Attempting to print an invalid RBinJavaSourceDebugExtensionAttr *.\n");
		return;
	}
	printf ("Source Debug Extension Attribute Information:\n");
	printf ("  Attribute Offset: 0x%08"PFMT64x "\n", attr->file_offset);
	printf ("  Attribute Name Index: %d (%s)\n", attr->name_idx, attr->name);
	printf ("  Extension Length: %d\n", attr->length);
	printf ("  Source Debug Extension value: \n");
	for (i = 0; i < attr->length; i++) {
		printf ("%c", attr->info.debug_extensions.debug_extension[i]);
	}
	printf ("\n  Source Debug Extension End\n");
}