R_API RList *r_bin_java_get_sections(RBinJavaObj *bin) {
	RBinSection *section = NULL;
	RList *sections = r_list_newf (free);
	ut64 baddr = bin->loadaddr;
	RBinJavaField *fm_type;
	RListIter *iter = NULL;
	if (bin->cp_count > 0) {
		section = R_NEW0 (RBinSection);
		if (section) {
			section->name = strdup ("constant_pool");
			section->paddr = bin->cp_offset + baddr;
			section->size = bin->cp_size;
#if 0
			section->vsize = section->size;
			section->vaddr = 0x10; // XXX // bin->cp_offset; //  + baddr;
#endif
			section->vaddr = baddr;
			// section->vaddr = section->paddr;
			// section->vsize = section->size;
			section->perm = R_PERM_R;
			section->add = true;
			r_list_append (sections, section);
		}
		section = NULL;
	}
	if (bin->fields_count > 0) {
		section = R_NEW0 (RBinSection);
		if (section) {
			section->name = strdup ("fields");
			section->size = bin->fields_size;
			section->paddr = bin->fields_offset + baddr;
#if 0
			section->vsize = section->size;
			section->vaddr = section->paddr;
#endif
			section->perm = R_PERM_R;
			section->add = true;
			r_list_append (sections, section);
			section = NULL;
			r_list_foreach (bin->fields_list, iter, fm_type) {
				if (fm_type->attr_offset == 0) {
					continue;
				}
				section = R_NEW0 (RBinSection);
				if (section) {
					section->name = r_str_newf ("attrs.%s", fm_type->name);
					section->size = fm_type->size - (fm_type->file_offset - fm_type->attr_offset);
#if 0
					section->vsize = section->size;
					section->vaddr = section->paddr;
#endif
					section->paddr = fm_type->attr_offset + baddr;
					section->perm = R_PERM_R;
					section->add = true;
					r_list_append (sections, section);
				}
			}
		}
	}
	if (bin->methods_count > 0) {
		section = R_NEW0 (RBinSection);
		if (section) {
			section->name = strdup ("methods");
			section->paddr = bin->methods_offset + baddr;
			section->size = bin->methods_size;
			// section->vaddr = section->paddr;
			// section->vsize = section->size;
			section->perm = R_PERM_RX;
			section->add = true;
			r_list_append (sections, section);
			section = NULL;
			r_list_foreach (bin->methods_list, iter, fm_type) {
				if (fm_type->attr_offset == 0) {
					continue;
				}
				section = R_NEW0 (RBinSection);
				if (section) {
					section->name = r_str_newf ("attrs.%s", fm_type->name);
					section->size = fm_type->size - (fm_type->file_offset - fm_type->attr_offset);
					// section->vsize = section->size;
					// section->vaddr = section->paddr;
					section->paddr = fm_type->attr_offset + baddr;
					section->perm = R_PERM_R | R_PERM_X;
					section->add = true;
					r_list_append (sections, section);
				}
			}
		}
	}
	if (bin->interfaces_count > 0) {
		section = R_NEW0 (RBinSection);
		if (section) {
			section->name = strdup ("interfaces");
			section->paddr = bin->interfaces_offset + baddr;
			section->size = bin->interfaces_size;
			// section->vaddr = section->paddr;
			// section->vsize = section->size;
			section->perm = R_PERM_R;
			section->add = true;
			r_list_append (sections, section);
		}
		section = NULL;
	}
	if (bin->attrs_count > 0) {
		section = R_NEW0 (RBinSection);
		if (section) {
			section->name = strdup ("attributes");
			section->paddr = bin->attrs_offset + baddr;
			section->size = bin->attrs_size;
			// section->vaddr = section->paddr;
			// section->vsize = section->size;
			section->perm = R_PERM_R;
			section->perm = R_PERM_R;
			section->add = true;
			r_list_append (sections, section);
		}
		section = NULL;
	}
	return sections;
}