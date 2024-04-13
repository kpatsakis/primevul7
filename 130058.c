mono_image_fill_export_table (MonoDomain *domain, MonoReflectionTypeBuilder *tb,
	guint32 module_index, guint32 parent_index, MonoDynamicImage *assembly)
{
	MonoClass *klass;
	guint32 idx, i;

	klass = mono_class_from_mono_type (mono_reflection_type_get_handle ((MonoReflectionType*)tb));

	klass->type_token = mono_metadata_make_token (MONO_TABLE_TYPEDEF, tb->table_idx);

	idx = mono_image_fill_export_table_from_class (domain, klass, module_index, 
												   parent_index, assembly);

	/* 
	 * Emit nested types
	 * We need to do this ourselves since klass->nested_classes is not set up.
	 */
	if (tb->subtypes) {
		for (i = 0; i < mono_array_length (tb->subtypes); ++i)
			mono_image_fill_export_table (domain, mono_array_get (tb->subtypes, MonoReflectionTypeBuilder*, i), module_index, idx, assembly);
	}
}