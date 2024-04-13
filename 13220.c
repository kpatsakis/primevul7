R_API void r_bin_java_print_code_exceptions_attr_summary(RBinJavaExceptionEntry *exc_entry) {
	if (exc_entry == NULL) {
		eprintf ("Attempting to print an invalid RBinJavaExceptionEntry *.\n");
		return;
	}
	printf ("  Exception Table Entry Information\n");
	printf ("    offset:	0x%08"PFMT64x"\n", exc_entry->file_offset);
	printf ("    catch_type: %d\n", exc_entry->catch_type);
	printf ("    start_pc:   0x%04x\n", exc_entry->start_pc);
	printf ("    end_pc:	0x%04x\n", exc_entry->end_pc);
	printf ("    handler_pc: 0x%04x\n", exc_entry->handler_pc);
}