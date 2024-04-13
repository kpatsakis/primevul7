gpointer
mono_reflection_lookup_dynamic_token (MonoImage *image, guint32 token, gboolean valid_token, MonoClass **handle_class, MonoGenericContext *context)
{
	MonoDynamicImage *assembly = (MonoDynamicImage*)image;
	MonoObject *obj;
	MonoClass *klass;

	mono_loader_lock ();
	obj = mono_g_hash_table_lookup (assembly->tokens, GUINT_TO_POINTER (token));
	mono_loader_unlock ();
	if (!obj) {
		if (valid_token)
			g_error ("Could not find required dynamic token 0x%08x", token);
		else
			return NULL;
	}

	if (!handle_class)
		handle_class = &klass;
	return resolve_object (image, obj, handle_class, context);