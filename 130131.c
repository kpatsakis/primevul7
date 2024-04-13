mono_image_build_metadata (MonoReflectionModuleBuilder *moduleb)
{
	MonoDynamicTable *table;
	MonoDynamicImage *assembly;
	MonoReflectionAssemblyBuilder *assemblyb;
	MonoDomain *domain;
	MonoPtrArray types;
	guint32 *values;
	int i, j;

	assemblyb = moduleb->assemblyb;
	assembly = moduleb->dynamic_image;
	domain = mono_object_domain (assemblyb);

	if (assembly->text_rva)
		return;

	assembly->text_rva = START_TEXT_RVA;

	if (moduleb->is_main) {
		mono_image_emit_manifest (moduleb);
	}

	table = &assembly->tables [MONO_TABLE_TYPEDEF];
	table->rows = 1; /* .<Module> */
	table->next_idx++;
	alloc_table (table, table->rows);
	/*
	 * Set the first entry.
	 */
	values = table->values + table->columns;
	values [MONO_TYPEDEF_FLAGS] = 0;
	values [MONO_TYPEDEF_NAME] = string_heap_insert (&assembly->sheap, "<Module>") ;
	values [MONO_TYPEDEF_NAMESPACE] = string_heap_insert (&assembly->sheap, "") ;
	values [MONO_TYPEDEF_EXTENDS] = 0;
	values [MONO_TYPEDEF_FIELD_LIST] = 1;
	values [MONO_TYPEDEF_METHOD_LIST] = 1;

	/* 
	 * handle global methods 
	 * FIXME: test what to do when global methods are defined in multiple modules.
	 */
	if (moduleb->global_methods) {
		table = &assembly->tables [MONO_TABLE_METHOD];
		table->rows += mono_array_length (moduleb->global_methods);
		alloc_table (table, table->rows);
		for (i = 0; i < mono_array_length (moduleb->global_methods); ++i)
			mono_image_get_method_info (
				mono_array_get (moduleb->global_methods, MonoReflectionMethodBuilder*, i), assembly);
	}
	if (moduleb->global_fields) {
		table = &assembly->tables [MONO_TABLE_FIELD];
		table->rows += mono_array_length (moduleb->global_fields);
		alloc_table (table, table->rows);
		for (i = 0; i < mono_array_length (moduleb->global_fields); ++i)
			mono_image_get_field_info (
				mono_array_get (moduleb->global_fields, MonoReflectionFieldBuilder*, i), assembly);
	}

	table = &assembly->tables [MONO_TABLE_MODULE];
	alloc_table (table, 1);
	mono_image_fill_module_table (domain, moduleb, assembly);

	/* Collect all types into a list sorted by their table_idx */
	mono_ptr_array_init (types, moduleb->num_types);

	if (moduleb->types)
		for (i = 0; i < moduleb->num_types; ++i) {
			MonoReflectionTypeBuilder *type = mono_array_get (moduleb->types, MonoReflectionTypeBuilder*, i);
			collect_types (&types, type);
		}

	mono_ptr_array_sort (types, (gpointer)compare_types_by_table_idx);
	table = &assembly->tables [MONO_TABLE_TYPEDEF];
	table->rows += mono_ptr_array_size (types);
	alloc_table (table, table->rows);

	/*
	 * Emit type names + namespaces at one place inside the string heap,
	 * so load_class_names () needs to touch fewer pages.
	 */
	for (i = 0; i < mono_ptr_array_size (types); ++i) {
		MonoReflectionTypeBuilder *tb = mono_ptr_array_get (types, i);
		string_heap_insert_mstring (&assembly->sheap, tb->nspace);
	}
	for (i = 0; i < mono_ptr_array_size (types); ++i) {
		MonoReflectionTypeBuilder *tb = mono_ptr_array_get (types, i);
		string_heap_insert_mstring (&assembly->sheap, tb->name);
	}

	for (i = 0; i < mono_ptr_array_size (types); ++i) {
		MonoReflectionTypeBuilder *type = mono_ptr_array_get (types, i);
		mono_image_get_type_info (domain, type, assembly);
	}

	/* 
	 * table->rows is already set above and in mono_image_fill_module_table.
	 */
	/* add all the custom attributes at the end, once all the indexes are stable */
	mono_image_add_cattrs (assembly, 1, MONO_CUSTOM_ATTR_ASSEMBLY, assemblyb->cattrs);

	/* CAS assembly permissions */
	if (assemblyb->permissions_minimum)
		mono_image_add_decl_security (assembly, mono_metadata_make_token (MONO_TABLE_ASSEMBLY, 1), assemblyb->permissions_minimum);
	if (assemblyb->permissions_optional)
		mono_image_add_decl_security (assembly, mono_metadata_make_token (MONO_TABLE_ASSEMBLY, 1), assemblyb->permissions_optional);
	if (assemblyb->permissions_refused)
		mono_image_add_decl_security (assembly, mono_metadata_make_token (MONO_TABLE_ASSEMBLY, 1), assemblyb->permissions_refused);

	module_add_cattrs (assembly, moduleb);

	/* fixup tokens */
	mono_g_hash_table_foreach (assembly->token_fixups, (GHFunc)fixup_method, assembly);

	/* Create the MethodImpl table.  We do this after emitting all methods so we already know
	 * the final tokens and don't need another fixup pass. */

	if (moduleb->global_methods) {
		for (i = 0; i < mono_array_length (moduleb->global_methods); ++i) {
			MonoReflectionMethodBuilder *mb = mono_array_get (
				moduleb->global_methods, MonoReflectionMethodBuilder*, i);
			mono_image_add_methodimpl (assembly, mb);
		}
	}

	for (i = 0; i < mono_ptr_array_size (types); ++i) {
		MonoReflectionTypeBuilder *type = mono_ptr_array_get (types, i);
		if (type->methods) {
			for (j = 0; j < type->num_methods; ++j) {
				MonoReflectionMethodBuilder *mb = mono_array_get (
					type->methods, MonoReflectionMethodBuilder*, j);

				mono_image_add_methodimpl (assembly, mb);
			}
		}
	}

	mono_ptr_array_destroy (types);

	fixup_cattrs (assembly);
}