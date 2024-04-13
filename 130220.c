static void
typebuilder_setup_properties (MonoClass *klass, MonoError *error)
{
	MonoReflectionTypeBuilder *tb = mono_class_get_ref_info (klass);
	MonoReflectionPropertyBuilder *pb;
	MonoImage *image = klass->image;
	MonoProperty *properties;
	int i;

	mono_error_init (error);

	if (!klass->ext)
		klass->ext = image_g_new0 (image, MonoClassExt, 1);

	klass->ext->property.count = tb->properties ? mono_array_length (tb->properties) : 0;
	klass->ext->property.first = 0;

	properties = image_g_new0 (image, MonoProperty, klass->ext->property.count);
	klass->ext->properties = properties;
	for (i = 0; i < klass->ext->property.count; ++i) {
		pb = mono_array_get (tb->properties, MonoReflectionPropertyBuilder*, i);
		properties [i].parent = klass;
		properties [i].attrs = pb->attrs;
		properties [i].name = mono_string_to_utf8_image (image, pb->name, error);
		if (!mono_error_ok (error))
			return;
		if (pb->get_method)
			properties [i].get = pb->get_method->mhandle;
		if (pb->set_method)
			properties [i].set = pb->set_method->mhandle;

		mono_save_custom_attrs (klass->image, &properties [i], pb->cattrs);
		if (pb->def_value) {
			guint32 len, idx;
			const char *p, *p2;
			MonoDynamicImage *assembly = (MonoDynamicImage*)klass->image;
			if (!klass->ext->prop_def_values)
				klass->ext->prop_def_values = image_g_new0 (image, MonoFieldDefaultValue, klass->ext->property.count);
			properties [i].attrs |= PROPERTY_ATTRIBUTE_HAS_DEFAULT;
			idx = encode_constant (assembly, pb->def_value, &klass->ext->prop_def_values [i].def_type);
			/* Copy the data from the blob since it might get realloc-ed */
			p = assembly->blob.data + idx;
			len = mono_metadata_decode_blob_size (p, &p2);
			len += p2 - p;
			klass->ext->prop_def_values [i].data = mono_image_alloc (image, len);
			memcpy ((gpointer)klass->ext->prop_def_values [i].data, p, len);
		}
	}