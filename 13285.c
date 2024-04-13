R_API void r_bin_java_print_local_variable_attr_summary(RBinJavaLocalVariableAttribute *lvattr) {
	if (!lvattr) {
		eprintf ("Attempting to print an invalid RBinJavaLocalVariableAttribute *.\n");
		return;
	}
	printf ("  Local Variable Attribute offset: 0x%08"PFMT64x "\n", lvattr->file_offset);
	printf ("  Local Variable Attribute start_pc: %d\n", lvattr->start_pc);
	printf ("  Local Variable Attribute Length: %d\n", lvattr->length);
	printf ("  Local Variable Attribute name_idx: %d\n", lvattr->name_idx);
	printf ("  Local Variable Attribute name: %s\n", lvattr->name);
	printf ("  Local Variable Attribute descriptor_idx: %d\n", lvattr->descriptor_idx);
	printf ("  Local Variable Attribute descriptor: %s\n", lvattr->descriptor);
	printf ("  Local Variable Attribute index: %d\n", lvattr->index);
}