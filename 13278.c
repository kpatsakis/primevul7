R_API void r_bin_java_get_class_info_json(RBinJavaObj *bin, PJ *pj) {
	r_return_if_fail (bin && pj);
	RList *classes = r_bin_java_get_classes (bin);
	RBinClass *class_ = r_list_get_n (classes, 0);

	// pj dict already opened

	if (class_) {
		int dummy = 0;
		RListIter *iter;
		RBinClass *class_v = NULL;
		// add access flags like in methods
		bool is_public = ((class_->visibility & R_BIN_JAVA_CLASS_ACC_PUBLIC) != 0);
		bool is_final = ((class_->visibility & R_BIN_JAVA_CLASS_ACC_FINAL) != 0);
		bool is_super = ((class_->visibility & R_BIN_JAVA_CLASS_ACC_SUPER) != 0);
		bool is_interface = ((class_->visibility & R_BIN_JAVA_CLASS_ACC_INTERFACE) != 0);
		bool is_abstract = ((class_->visibility & R_BIN_JAVA_CLASS_ACC_ABSTRACT) != 0);
		bool is_synthetic = ((class_->visibility & R_BIN_JAVA_CLASS_ACC_SYNTHETIC) != 0);
		bool is_annotation = ((class_->visibility & R_BIN_JAVA_CLASS_ACC_ANNOTATION) != 0);
		bool is_enum = ((class_->visibility & R_BIN_JAVA_CLASS_ACC_ENUM) != 0);

		pj_ki (pj, "access_flags", class_->visibility);
		pj_ki (pj, "is_public", is_public);
		pj_ki (pj, "is_final", is_final);
		pj_ki (pj, "is_super", is_super);
		pj_ki (pj, "is_interface", is_interface);
		pj_ki (pj, "is_abstract", is_abstract);
		pj_ki (pj, "is_synthetic", is_synthetic);
		pj_ki (pj, "is_annotation", is_annotation);
		pj_ki (pj, "is_enum", is_enum);
		pj_ks (pj, "name", class_->name);

		pj_ks (pj, "super", r_str_get (class_->super));

		pj_ka (pj, "interfaces");

		r_list_foreach (classes, iter, class_v) {
			if (!dummy) {
				dummy++;
				continue;
			}
			// enumerate all interface classes and append them to the interfaces
			if ((class_v->visibility & R_BIN_JAVA_CLASS_ACC_INTERFACE) != 0) {
				pj_s (pj, class_v->name);
			}
		}

		pj_end (pj);
	}
	r_list_free (classes);
}