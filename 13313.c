R_API void r_bin_java_bootstrap_method_argument_free(void /*RBinJavaBootStrapArgument*/ *b) {
	RBinJavaBootStrapArgument *bsm_arg = b;
	if (bsm_arg) {
		RBinJavaCPTypeMetas *tm = (RBinJavaCPTypeMetas*)bsm_arg->argument_info_cp_obj;
		if (tm) {
			if (tm && (size_t)(tm->allocs) > 1024 && tm->allocs->delete_obj) {
				tm->allocs->delete_obj (tm);
			}
			bsm_arg->argument_info_cp_obj = NULL;
		}
		free (bsm_arg);
	}
}