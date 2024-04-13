R_API ut64 r_bin_java_rti_annotations_attr_calc_size(RBinJavaAttrInfo *attr) {
	ut64 size = 0;
	if (!attr) {
		// TODO eprintf allocation fail
		return size;
	}
	size += (6 + r_bin_java_annotation_array_calc_size (&(attr->info.annotation_array)));
	return size;
}