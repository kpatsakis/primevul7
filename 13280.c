R_API RBinJavaObj *r_bin_java_new(const char *file, ut64 loadaddr, Sdb *kv) {
	RBinJavaObj *bin = R_NEW0 (RBinJavaObj);
	if (!bin) {
		return NULL;
	}
	bin->file = strdup (file);
	size_t sz;
	ut8 *buf = (ut8 *)r_file_slurp (file, &sz);
	bin->size = sz;
	if (!buf) {
		return r_bin_java_free (bin);
	}
	if (!r_bin_java_new_bin (bin, loadaddr, kv, buf, bin->size)) {
		r_bin_java_free (bin);
		bin = NULL;
	}
	free (buf);
	return bin;
}