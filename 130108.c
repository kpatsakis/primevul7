module_add_cattrs (MonoDynamicImage *assembly, MonoReflectionModuleBuilder *moduleb)
{
	int i;
	
	mono_image_add_cattrs (assembly, moduleb->table_idx, MONO_CUSTOM_ATTR_MODULE, moduleb->cattrs);

	if (moduleb->global_methods) {
		for (i = 0; i < mono_array_length (moduleb->global_methods); ++i) {
			MonoReflectionMethodBuilder* mb = mono_array_get (moduleb->global_methods, MonoReflectionMethodBuilder*, i);
			mono_image_add_cattrs (assembly, mb->table_idx, MONO_CUSTOM_ATTR_METHODDEF, mb->cattrs);
			params_add_cattrs (assembly, mb->pinfo);
		}
	}

	if (moduleb->global_fields) {
		for (i = 0; i < mono_array_length (moduleb->global_fields); ++i) {
			MonoReflectionFieldBuilder *fb = mono_array_get (moduleb->global_fields, MonoReflectionFieldBuilder*, i);
			mono_image_add_cattrs (assembly, fb->table_idx, MONO_CUSTOM_ATTR_FIELDDEF, fb->cattrs);
		}
	}
	
	if (moduleb->types) {
		for (i = 0; i < moduleb->num_types; ++i)
			type_add_cattrs (assembly, mono_array_get (moduleb->types, MonoReflectionTypeBuilder*, i));
	}
}