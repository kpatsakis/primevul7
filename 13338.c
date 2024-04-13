R_API RBinField *r_bin_java_allocate_rbinfield(void) {
	RBinField *t = (RBinField *) malloc (sizeof (RBinField));
	if (t) {
		memset (t, 0, sizeof (RBinField));
	}
	return t;
}