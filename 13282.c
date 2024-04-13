R_API RBinJavaObj *r_bin_java_new_buf(RBuffer *buf, ut64 loadaddr, Sdb *kv) {
	RBinJavaObj *bin = R_NEW0 (RBinJavaObj);
	if (!bin) {
		return NULL;
	}
	ut64 tmpsz;
	const ut8 *tmp = r_buf_data (buf, &tmpsz);
	if (!r_bin_java_new_bin (bin, loadaddr, kv, tmp, tmpsz)) {
		return r_bin_java_free (bin);
	}
	return bin;
}