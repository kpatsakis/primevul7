mono_image_register_token (MonoDynamicImage *assembly, guint32 token, MonoObject *obj)
{
	MonoObject *prev = mono_g_hash_table_lookup (assembly->tokens, GUINT_TO_POINTER (token));
	if (prev) {
		/* There could be multiple MethodInfo objects with the same token */
		//g_assert (prev == obj);
	} else {
		mono_g_hash_table_insert (assembly->tokens, GUINT_TO_POINTER (token), obj);
	}
}