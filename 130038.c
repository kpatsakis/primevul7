mono_image_fill_export_table_from_module (MonoDomain *domain, MonoReflectionModule *module,
	guint32 module_index, MonoDynamicImage *assembly)
{
	MonoImage *image = module->image;
	MonoTableInfo  *t;
	guint32 i;

	t = &image->tables [MONO_TABLE_TYPEDEF];

	for (i = 0; i < t->rows; ++i) {
		MonoClass *klass = mono_class_get (image, mono_metadata_make_token (MONO_TABLE_TYPEDEF, i + 1));

		if (klass->flags & TYPE_ATTRIBUTE_PUBLIC)
			mono_image_fill_export_table_from_class (domain, klass, module_index, 0, assembly);
	}
}