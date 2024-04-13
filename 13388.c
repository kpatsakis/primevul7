R_API void r_bin_java_print_field_summary(RBinJavaField *field) {
	RBinJavaAttrInfo *attr;
	RListIter *iter, *iter_tmp;
	if (field) {
		if (field->type == R_BIN_JAVA_FIELD_TYPE_METHOD) {
			r_bin_java_print_method_summary (field);
		} else {
#if 0
			r_bin_java_print_interface_summary (field);
#else
			printf ("Field Summary Information:\n");
			printf ("  File Offset: 0x%08"PFMT64x "\n", field->file_offset);
			printf ("  Name Index: %d (%s)\n", field->name_idx, field->name);
			printf ("  Descriptor Index: %d (%s)\n", field->descriptor_idx, field->descriptor);
			printf ("  Access Flags: 0x%02x (%s)\n", field->flags, field->flags_str);
			printf ("  Field Attributes Count: %d\n", field->attr_count);
			printf ("  Field Attributes:\n");
			r_list_foreach_safe (field->attributes, iter, iter_tmp, attr) {
				r_bin_java_print_attr_summary (attr);
			}
#endif
		}
	} else {
		eprintf ("Attempting to print an invalid RBinJavaField* Field.\n");
	}
}