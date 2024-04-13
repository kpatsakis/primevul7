encode_generic_method_definition_sig (MonoDynamicImage *assembly, MonoReflectionMethodBuilder *mb)
{
	SigBuffer buf;
	int i;
	guint32 nparams = mono_array_length (mb->generic_params);
	guint32 idx;

	if (!assembly->save)
		return 0;

	sigbuffer_init (&buf, 32);

	sigbuffer_add_value (&buf, 0xa);
	sigbuffer_add_value (&buf, nparams);

	for (i = 0; i < nparams; i++) {
		sigbuffer_add_value (&buf, MONO_TYPE_MVAR);
		sigbuffer_add_value (&buf, i);
	}

	idx = sigbuffer_add_to_blob_cached (assembly, &buf);
	sigbuffer_free (&buf);
	return idx;
}