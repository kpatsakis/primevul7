R_API ut64 r_bin_java_element_pair_calc_size(RBinJavaElementValuePair *evp) {
	ut64 sz = 2;
	if (evp && evp->value) {
		// evp->element_name_idx = r_bin_java_read_short(bin, bin->b->cur);
		// evp->value = r_bin_java_element_value_new (bin, offset+2);
		sz += r_bin_java_element_value_calc_size (evp->value);
	}
	return sz;
}