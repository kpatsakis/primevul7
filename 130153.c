MonoMethodSignature *
mono_reflection_lookup_signature (MonoImage *image, MonoMethod *method, guint32 token)
{
	MonoMethodSignature *sig;
	g_assert (image->dynamic);

	sig = g_hash_table_lookup (((MonoDynamicImage*)image)->vararg_aux_hash, GUINT_TO_POINTER (token));
	if (sig)
		return sig;

	return mono_method_signature (method);