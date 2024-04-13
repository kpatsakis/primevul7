R_API void r_bin_java_print_method_summary(RBinJavaField *field) {
	RBinJavaAttrInfo *attr;
	RListIter *iter, *iter_tmp;
	if (field == NULL) {
		eprintf ("Attempting to print an invalid RBinJavaField* Method.\n");
		return;
	}
	printf ("Method Summary Information:\n");
	printf ("  File Offset: 0x%08"PFMT64x "\n", field->file_offset);
	printf ("  Name Index: %d (%s)\n", field->name_idx, field->name);
	printf ("  Descriptor Index: %d (%s)\n", field->descriptor_idx, field->descriptor);
	printf ("  Access Flags: 0x%02x (%s)\n", field->flags, field->flags_str);
	printf ("  Method Attributes Count: %d\n", field->attr_count);
	printf ("  Method Attributes:\n");
	r_list_foreach_safe (field->attributes, iter, iter_tmp, attr) {
		r_bin_java_print_attr_summary (attr);
	}
}