R_API RList *r_bin_java_get_symbols(RBinJavaObj *bin) {
	RListIter *iter = NULL, *iter_tmp = NULL;
	RList *imports, *symbols = r_list_newf (free);
	RBinSymbol *sym = NULL;
	RBinImport *imp;
	RBinJavaField *fm_type;
	r_list_foreach_safe (bin->methods_list, iter, iter_tmp, fm_type) {
		sym = r_bin_java_create_new_symbol_from_field (fm_type, bin->loadaddr);
		if (sym) {
			r_list_append (symbols, (void *) sym);
		}
		sym = r_bin_java_create_new_symbol_from_fm_type_meta (fm_type, bin->loadaddr);
		if (sym) {
			r_list_append (symbols, (void *) sym);
		}
	}
	r_list_foreach_safe (bin->fields_list, iter, iter_tmp, fm_type) {
		sym = r_bin_java_create_new_symbol_from_field (fm_type, bin->loadaddr);
		if (sym) {
			r_list_append (symbols, (void *) sym);
		}
		sym = r_bin_java_create_new_symbol_from_fm_type_meta (fm_type, bin->loadaddr);
		if (sym) {
			r_list_append (symbols, (void *) sym);
		}
	}
	bin->lang = "java";
	if (bin->cf.major[1] >= 46) {
		switch (bin->cf.major[1]) {
			static char lang[32];
			int langid;
			case 46:
			case 47:
			case 48:
				langid = 2 + (bin->cf.major[1] - 46);
				snprintf (lang, sizeof (lang) - 1, "java 1.%d", langid);
				bin->lang = lang;
				break;
			default:
				langid = 5 + (bin->cf.major[1] - 49);
				snprintf (lang, sizeof (lang) - 1, "java %d", langid);
				bin->lang = lang;
		}
	}
	imports = r_bin_java_get_imports (bin);
	r_list_foreach (imports, iter, imp) {
		sym = R_NEW0 (RBinSymbol);
		if (!sym) {
			break;
		}
		if (imp->classname && !strncmp (imp->classname, "kotlin/jvm", 10)) {
			bin->lang = "kotlin";
		}
		sym->name = strdup (imp->name);
		sym->is_imported = true;
		if (!sym->name) {
			free (sym);
			break;
		}
		sym->type = "import";
		if (!sym->type) {
			free (sym);
			break;
		}
		sym->vaddr = sym->paddr = imp->ordinal;
		sym->ordinal = imp->ordinal;
		r_list_append (symbols, (void *) sym);
	}
	r_list_free (imports);
	return symbols;
}