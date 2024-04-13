R_API RBinJavaAttrInfo *r_bin_java_deprecated_attr_new(RBinJavaObj *bin, ut8 *buffer, ut64 sz, ut64 buf_offset) {
	RBinJavaAttrInfo *attr = r_bin_java_default_attr_new (bin, buffer, sz, buf_offset);
	if (attr) {
		attr->type = R_BIN_JAVA_ATTR_TYPE_DEPRECATED_ATTR;
		attr->size = 6;
	}
	// IFDBG r_bin_java_print_deprecated_attr_summary(attr);
	return attr;
}