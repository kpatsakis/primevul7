R_API void r_bin_java_element_pair_free(void /*RBinJavaElementValuePair*/ *e) {
	RBinJavaElementValuePair *evp = e;
	if (evp) {
		free (evp->name);
		r_bin_java_element_value_free (evp->value);
		free (evp);
	}
	evp = NULL;
}