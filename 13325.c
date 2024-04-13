R_API void r_bin_java_print_stack_map_table_attr_summary(RBinJavaAttrInfo *attr) {
	RListIter *iter, *iter_tmp;
	RBinJavaStackMapFrame *frame;
	if (!attr) {
		eprintf ("Attempting to print an invalid RBinJavaStackMapTableAttr*  .\n");
		return;
	}
	printf ("StackMapTable Attribute Information:\n");
	printf ("  Attribute Offset: 0x%08"PFMT64x "\n", attr->file_offset);
	printf ("  Attribute Name Index: %d (%s)\n", attr->name_idx, attr->name);
	printf ("  Attribute Length: %d\n", attr->length);
	printf ("  StackMapTable Method Code Size: 0x%08x\n", attr->info.stack_map_table_attr.code_size);
	printf ("  StackMapTable Frame Entries: 0x%08x\n", attr->info.stack_map_table_attr.number_of_entries);
	printf ("  StackMapTable Frames:\n");
	RList *ptrList = attr->info.stack_map_table_attr.stack_map_frame_entries;
	if (ptrList) {
		r_list_foreach_safe (ptrList, iter, iter_tmp, frame) {
			r_bin_java_print_stack_map_frame_summary (frame);
		}
	}
}