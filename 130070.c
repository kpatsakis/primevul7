mono_image_get_ctorbuilder_token (MonoDynamicImage *assembly, MonoReflectionCtorBuilder *mb)
{
	guint32 token, parent, sig;
	ReflectionMethodBuilder rmb;
	char *name;
	MonoReflectionTypeBuilder *tb = (MonoReflectionTypeBuilder *)mb->type;
	
	token = GPOINTER_TO_UINT (mono_g_hash_table_lookup (assembly->handleref_managed, mb));
	if (token)
		return token;

	g_assert (tb->generic_params);

	reflection_methodbuilder_from_ctor_builder (&rmb, mb);

	parent = create_generic_typespec (assembly, tb);
	name = mono_string_to_utf8 (rmb.name);
	sig = method_builder_encode_signature (assembly, &rmb);

	token = mono_image_add_memberef_row (assembly, parent, name, sig);

	g_free (name);
	mono_g_hash_table_insert (assembly->handleref_managed, mb, GUINT_TO_POINTER(token));
	return token;
}