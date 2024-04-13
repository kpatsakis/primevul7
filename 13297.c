R_API void U(r_bin_java_print_stack_map_append_frame_summary)(RBinJavaStackMapFrame * obj) {
	RListIter *iter, *iter_tmp;
	RList *ptrList;
	RBinJavaVerificationObj *ver_obj;
	printf ("Stack Map Frame Information\n");
	printf ("  Tag Value = 0x%02x Name: %s\n", obj->tag, ((RBinJavaStackMapFrameMetas *) obj->metas->type_info)->name);
	printf ("  Offset: 0x%08"PFMT64x "\n", obj->file_offset);
	printf ("  Local Variable Count = 0x%04x\n", obj->number_of_locals);
	printf ("  Local Variables:\n");
	ptrList = obj->local_items;
	r_list_foreach_safe (ptrList, iter, iter_tmp, ver_obj) {
		r_bin_java_print_verification_info_summary (ver_obj);
	}
	printf ("  Stack Items Count = 0x%04x\n", obj->number_of_stack_items);
	printf ("  Stack Items:\n");
	ptrList = obj->stack_items;
	r_list_foreach_safe (ptrList, iter, iter_tmp, ver_obj) {
		r_bin_java_print_verification_info_summary (ver_obj);
	}
}