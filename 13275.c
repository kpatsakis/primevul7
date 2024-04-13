R_API bool r_bin_java_is_old_format(RBinJavaObj *bin) {
	return bin->cf.major[1] == 45 && bin->cf.minor[1] <= 2;
}