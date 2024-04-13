R_API void r_bin_java_print_signature_attr_summary(RBinJavaAttrInfo *attr) {
	if (!attr) {
		eprintf ("Attempting to print an invalid RBinJavaAttrInfo *SignatureAttr.\n");
		return;
	}
	printf ("Signature Attribute Information:\n");
	printf ("  Attribute Offset: 0x%08"PFMT64x "\n", attr->file_offset);
	printf ("  Attribute Name Index: %d (%s)\n", attr->name_idx, attr->name);
	printf ("  Attribute Length: %d\n", attr->length);
	printf ("  Signature UTF8 Index: %d\n", attr->info.signature_attr.signature_idx);
	printf ("  Signature string: %s\n", attr->info.signature_attr.signature);
}