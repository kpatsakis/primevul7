R_API void r_bin_java_print_enclosing_methods_attr_summary(RBinJavaAttrInfo *attr) {
	if (!attr) {
		eprintf ("Attempting to print an invalid RBinJavaAttrInfo *Deperecated.\n");
		return;
	}
	printf ("Enclosing Method Attribute Information:\n");
	printf ("  Attribute Offset: 0x%08"PFMT64x "\n", attr->file_offset);
	printf ("  Attribute Name Index: %d (%s)\n", attr->name_idx, attr->name);
	printf ("  Attribute Length: %d\n", attr->length);
	printf ("  Class Info Index : 0x%02x\n", attr->info.enclosing_method_attr.class_idx);
	printf ("  Method Name and Type Index : 0x%02x\n", attr->info.enclosing_method_attr.method_idx);
	printf ("  Class Name : %s\n", attr->info.enclosing_method_attr.class_name);
	printf ("  Method Name and Desc : %s %s\n", attr->info.enclosing_method_attr.method_name, attr->info.enclosing_method_attr.method_descriptor);
}