mono_image_emit_manifest (MonoReflectionModuleBuilder *moduleb)
{
	MonoDynamicTable *table;
	MonoDynamicImage *assembly;
	MonoReflectionAssemblyBuilder *assemblyb;
	MonoDomain *domain;
	guint32 *values;
	int i;
	guint32 module_index;

	assemblyb = moduleb->assemblyb;
	assembly = moduleb->dynamic_image;
	domain = mono_object_domain (assemblyb);

	/* Emit ASSEMBLY table */
	table = &assembly->tables [MONO_TABLE_ASSEMBLY];
	alloc_table (table, 1);
	values = table->values + MONO_ASSEMBLY_SIZE;
	values [MONO_ASSEMBLY_HASH_ALG] = assemblyb->algid? assemblyb->algid: ASSEMBLY_HASH_SHA1;
	values [MONO_ASSEMBLY_NAME] = string_heap_insert_mstring (&assembly->sheap, assemblyb->name);
	if (assemblyb->culture) {
		values [MONO_ASSEMBLY_CULTURE] = string_heap_insert_mstring (&assembly->sheap, assemblyb->culture);
	} else {
		values [MONO_ASSEMBLY_CULTURE] = string_heap_insert (&assembly->sheap, "");
	}
	values [MONO_ASSEMBLY_PUBLIC_KEY] = load_public_key (assemblyb->public_key, assembly);
	values [MONO_ASSEMBLY_FLAGS] = assemblyb->flags;
	set_version_from_string (assemblyb->version, values);

	/* Emit FILE + EXPORTED_TYPE table */
	module_index = 0;
	for (i = 0; i < mono_array_length (assemblyb->modules); ++i) {
		int j;
		MonoReflectionModuleBuilder *file_module = 
			mono_array_get (assemblyb->modules, MonoReflectionModuleBuilder*, i);
		if (file_module != moduleb) {
			mono_image_fill_file_table (domain, (MonoReflectionModule*)file_module, assembly);
			module_index ++;
			if (file_module->types) {
				for (j = 0; j < file_module->num_types; ++j) {
					MonoReflectionTypeBuilder *tb = mono_array_get (file_module->types, MonoReflectionTypeBuilder*, j);
					mono_image_fill_export_table (domain, tb, module_index, 0, assembly);
				}
			}
		}
	}
	if (assemblyb->loaded_modules) {
		for (i = 0; i < mono_array_length (assemblyb->loaded_modules); ++i) {
			MonoReflectionModule *file_module = 
				mono_array_get (assemblyb->loaded_modules, MonoReflectionModule*, i);
			mono_image_fill_file_table (domain, file_module, assembly);
			module_index ++;
			mono_image_fill_export_table_from_module (domain, file_module, module_index, assembly);
		}
	}
	if (assemblyb->type_forwarders)
		mono_image_fill_export_table_from_type_forwarders (assemblyb, assembly);

	/* Emit MANIFESTRESOURCE table */
	module_index = 0;
	for (i = 0; i < mono_array_length (assemblyb->modules); ++i) {
		int j;
		MonoReflectionModuleBuilder *file_module = 
			mono_array_get (assemblyb->modules, MonoReflectionModuleBuilder*, i);
		/* The table for the main module is emitted later */
		if (file_module != moduleb) {
			module_index ++;
			if (file_module->resources) {
				int len = mono_array_length (file_module->resources);
				for (j = 0; j < len; ++j) {
					MonoReflectionResource* res = (MonoReflectionResource*)mono_array_addr (file_module->resources, MonoReflectionResource, j);
					assembly_add_resource_manifest (file_module, assembly, res, MONO_IMPLEMENTATION_FILE | (module_index << MONO_IMPLEMENTATION_BITS));
				}
			}
		}
	}		
}