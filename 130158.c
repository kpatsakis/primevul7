mono_reflection_get_type_internal_dynamic (MonoImage *rootimage, MonoAssembly *assembly, MonoTypeNameParse *info, gboolean ignorecase)
{
	MonoReflectionAssemblyBuilder *abuilder;
	MonoType *type;
	int i;

	g_assert (assembly->dynamic);
	abuilder = (MonoReflectionAssemblyBuilder*)mono_assembly_get_object (((MonoDynamicAssembly*)assembly)->domain, assembly);

	/* Enumerate all modules */

	type = NULL;
	if (abuilder->modules) {
		for (i = 0; i < mono_array_length (abuilder->modules); ++i) {
			MonoReflectionModuleBuilder *mb = mono_array_get (abuilder->modules, MonoReflectionModuleBuilder*, i);
			type = mono_reflection_get_type_internal (rootimage, &mb->dynamic_image->image, info, ignorecase);
			if (type)
				break;
		}
	}

	if (!type && abuilder->loaded_modules) {
		for (i = 0; i < mono_array_length (abuilder->loaded_modules); ++i) {
			MonoReflectionModule *mod = mono_array_get (abuilder->loaded_modules, MonoReflectionModule*, i);
			type = mono_reflection_get_type_internal (rootimage, mod->image, info, ignorecase);
			if (type)
				break;
		}
	}

	return type;
}