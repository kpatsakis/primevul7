mono_module_get_object   (MonoDomain *domain, MonoImage *image)
{
	static MonoClass *module_type;
	MonoReflectionModule *res;
	char* basename;
	
	CHECK_OBJECT (MonoReflectionModule *, image, NULL);
	if (!module_type) {
		MonoClass *class = mono_class_from_name (mono_defaults.corlib, "System.Reflection", "MonoModule");
		if (class == NULL)
			class = mono_class_from_name (mono_defaults.corlib, "System.Reflection", "Module");
		g_assert (class);
		module_type = class;
	}
	res = (MonoReflectionModule *)mono_object_new (domain, module_type);

	res->image = image;
	MONO_OBJECT_SETREF (res, assembly, (MonoReflectionAssembly *) mono_assembly_get_object(domain, image->assembly));

	MONO_OBJECT_SETREF (res, fqname, mono_string_new (domain, image->name));
	basename = g_path_get_basename (image->name);
	MONO_OBJECT_SETREF (res, name, mono_string_new (domain, basename));
	MONO_OBJECT_SETREF (res, scopename, mono_string_new (domain, image->module_name));
	
	g_free (basename);

	if (image->assembly->image == image) {
		res->token = mono_metadata_make_token (MONO_TABLE_MODULE, 1);
	} else {
		int i;
		res->token = 0;
		if (image->assembly->image->modules) {
			for (i = 0; i < image->assembly->image->module_count; i++) {
				if (image->assembly->image->modules [i] == image)
					res->token = mono_metadata_make_token (MONO_TABLE_MODULEREF, i + 1);
			}
			g_assert (res->token);
		}
	}

	CACHE_OBJECT (MonoReflectionModule *, image, res, NULL);
}