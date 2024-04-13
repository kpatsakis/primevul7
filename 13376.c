R_API RBinJavaAttrInfo *r_bin_java_unknown_attr_new(RBinJavaObj *bin, ut8 *buffer, ut64 sz, ut64 buf_offset) {
	return r_bin_java_default_attr_new (bin, buffer, sz, buf_offset);
}