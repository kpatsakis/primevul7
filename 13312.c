R_API void r_bin_java_print_element_pair_summary(RBinJavaElementValuePair *evp) {
	if (!evp) {
		eprintf ("Attempting to print an invalid RBinJavaElementValuePair *pair.\n");
		return;
	}
	printf ("Element Value Pair information:\n");
	printf ("  EV Pair File Offset: 0x%08"PFMT64x "\n", evp->file_offset);
	printf ("  EV Pair Element Name index: 0x%02x\n", evp->element_name_idx);
	printf ("  EV Pair Element Name: %s\n", evp->name);
	printf ("  EV Pair Element Value:\n");
	r_bin_java_print_element_value_summary (evp->value);
}