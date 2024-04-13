R_API RBinJavaAttrInfo *r_bin_java_source_code_file_attr_new(RBinJavaObj *bin, ut8 *buffer, ut64 sz, ut64 buf_offset) {
	if (!sz || sz == UT64_MAX) {
		return NULL;
	}
#if 0
	/// XXX this breaks tests
	if (sz < 8) {
		return NULL;
	}
#endif
	ut64 offset = 0;
	RBinJavaAttrInfo *attr = r_bin_java_default_attr_new (bin, buffer, sz, buf_offset);
	offset += 6;
	if (attr) {
		attr->type = R_BIN_JAVA_ATTR_TYPE_SOURCE_FILE_ATTR;
		attr->info.source_file_attr.sourcefile_idx = R_BIN_JAVA_USHORT (buffer, offset);
		offset += 2;
		attr->size = offset;
		// IFDBG r_bin_java_print_source_code_file_attr_summary(attr);
	}
	return attr;
}