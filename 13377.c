R_API void r_bin_java_print_stack_map_frame_summary(RBinJavaStackMapFrame *obj) {
	RListIter *iter, *iter_tmp;
	RBinJavaVerificationObj *ver_obj;
	if (!obj) {
		eprintf ("Attempting to print an invalid RBinJavaStackMapFrame*  .\n");
		return;
	}
	printf ("Stack Map Frame Information\n");
	printf ("  Tag Value = 0x%02x Name: %s\n", obj->tag, ((RBinJavaStackMapFrameMetas *) obj->metas->type_info)->name);
	printf ("  Offset: 0x%08"PFMT64x "\n", obj->file_offset);
	printf ("  Local Variable Count = 0x%04x\n", obj->number_of_locals);
	printf ("  Stack Items Count = 0x%04x\n", obj->number_of_stack_items);
	printf ("  Local Variables:\n");
	RList *ptrList = obj->local_items;
	r_list_foreach_safe (ptrList, iter, iter_tmp, ver_obj) {
		r_bin_java_print_verification_info_summary (ver_obj);
	}
	printf ("  Stack Items:\n");
	ptrList = obj->stack_items;
	r_list_foreach_safe (ptrList, iter, iter_tmp, ver_obj) {
		r_bin_java_print_verification_info_summary (ver_obj);
	}
}