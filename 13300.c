R_API void r_bin_java_print_synthetic_attr_summary(RBinJavaAttrInfo *attr) {
	if (attr == NULL) {
		eprintf ("Attempting to print an invalid RBinJavaAttrInfo *Synthetic.\n");
		return;
	}
	printf ("Synthetic Attribute Information:\n");
	printf ("  Attribute Offset: 0x%08"PFMT64x "\n", attr->file_offset);
	printf ("  Attribute Name Index: %d (%s)\n", attr->name_idx, attr->name);
	printf ("  Attribute Length: %d\n", attr->length);
	printf ("  Attribute Index: %d\n", attr->info.source_file_attr.sourcefile_idx);
}