R_API char *r_bin_java_get_item_desc_from_bin_cp_list(RBinJavaObj *bin, RBinJavaCPTypeObj *obj) {
	/*
	Given a constant pool object Class, FieldRef, MethodRef, or InterfaceMethodRef
	return the actual descriptor string.
	@param cp_list: RList of RBinJavaCPTypeObj *
	@param obj object to look up the name for
	@rvalue char* (user frees) or NULL
	*/
	return bin? r_bin_java_get_item_desc_from_cp_item_list (bin->cp_list, obj, MAX_CPITEMS): NULL;
}