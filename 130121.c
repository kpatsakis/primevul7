mono_image_get_ctor_on_inst_token (MonoDynamicImage *assembly, MonoReflectionCtorOnTypeBuilderInst *c, gboolean create_methodspec)
{
	guint32 sig, token;
	MonoClass *klass;
	MonoGenericClass *gclass;
	MonoType *type;

	/* A ctor cannot be a generic method, so we can ignore create_methodspec */

	token = GPOINTER_TO_UINT (mono_g_hash_table_lookup (assembly->handleref_managed, c));
	if (token)
		return token;

	if (is_sre_ctor_builder (mono_object_class (c->cb))) {
		MonoReflectionCtorBuilder *cb = (MonoReflectionCtorBuilder *)c->cb;
		MonoDynamicGenericClass *dgclass;
		ReflectionMethodBuilder rmb;
		char *name;

		type = mono_reflection_type_get_handle ((MonoReflectionType*)c->inst);
		klass = mono_class_from_mono_type (type);

		gclass = type->data.generic_class;
		g_assert (gclass->is_dynamic);
		dgclass = (MonoDynamicGenericClass *) gclass;

		reflection_methodbuilder_from_ctor_builder (&rmb, cb);

		name = mono_string_to_utf8 (rmb.name);

		sig = method_builder_encode_signature (assembly, &rmb);

		token = mono_image_get_memberref_token (assembly, &klass->byval_arg, name, sig);
		g_free (name);
	} else if (is_sr_mono_cmethod (mono_object_class (c->cb))) {
		MonoMethod *mm = ((MonoReflectionMethod *)c->cb)->method;

		type = mono_reflection_type_get_handle ((MonoReflectionType*)c->inst);
		klass = mono_class_from_mono_type (type);

		sig = method_encode_signature (assembly, mono_method_signature (mm));
		token = mono_image_get_memberref_token (assembly, &klass->byval_arg, mm->name, sig);
	} else {
		char *name = mono_type_get_full_name (mono_object_class (c->cb));
		g_error ("mono_image_get_method_on_inst_token: don't know how to handle %s", name);
	}


	mono_g_hash_table_insert (assembly->handleref_managed, c, GUINT_TO_POINTER (token));
	return token;
}