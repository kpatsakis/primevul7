mono_image_get_field_on_inst_token (MonoDynamicImage *assembly, MonoReflectionFieldOnTypeBuilderInst *f)
{
	guint32 token;
	MonoClass *klass;
	MonoGenericClass *gclass;
	MonoDynamicGenericClass *dgclass;
	MonoType *type;
	char *name;

	token = GPOINTER_TO_UINT (mono_g_hash_table_lookup (assembly->handleref_managed, f));
	if (token)
		return token;
	if (is_sre_field_builder (mono_object_class (f->fb))) {
		MonoReflectionFieldBuilder *fb = (MonoReflectionFieldBuilder *)f->fb;
		type = mono_reflection_type_get_handle ((MonoReflectionType*)f->inst);
		klass = mono_class_from_mono_type (type);
		gclass = type->data.generic_class;
		g_assert (gclass->is_dynamic);
		dgclass = (MonoDynamicGenericClass *) gclass;

		name = mono_string_to_utf8 (fb->name);
		token = mono_image_get_memberref_token (assembly, &klass->byval_arg, name, 
												field_encode_signature (assembly, fb));
		g_free (name);		
	} else if (is_sr_mono_field (mono_object_class (f->fb))) {
		guint32 sig;
		MonoClassField *field = ((MonoReflectionField *)f->fb)->field;

		type = mono_reflection_type_get_handle ((MonoReflectionType*)f->inst);
		klass = mono_class_from_mono_type (type);

		sig = fieldref_encode_signature (assembly, field->parent->image, field->type);
		token = mono_image_get_memberref_token (assembly, &klass->byval_arg, field->name, sig);
	} else {
		char *name = mono_type_get_full_name (mono_object_class (f->fb));
		g_error ("mono_image_get_field_on_inst_token: don't know how to handle %s", name);
	}

	mono_g_hash_table_insert (assembly->handleref_managed, f, GUINT_TO_POINTER (token));
	return token;
}