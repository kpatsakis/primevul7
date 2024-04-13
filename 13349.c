R_API void r_bin_java_print_rti_annotations_attr_summary(RBinJavaAttrInfo *attr) {
	if (attr && attr->type == R_BIN_JAVA_ATTR_TYPE_RUNTIME_INVISIBLE_ANNOTATION_ATTR) {
		printf ("Runtime Invisible Annotations Attribute Information:\n");
		printf ("   Attribute Offset: 0x%08"PFMT64x "\n", attr->file_offset);
		printf ("   Attribute Name Index: %d (%s)\n", attr->name_idx, attr->name);
		printf ("   Attribute Length: %d\n", attr->length);
		r_bin_java_print_annotation_array_summary (&attr->info.annotation_array);
	}
}