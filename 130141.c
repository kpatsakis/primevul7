field_encode_signature (MonoDynamicImage *assembly, MonoReflectionFieldBuilder *fb)
{
	SigBuffer buf;
	guint32 idx;

	sigbuffer_init (&buf, 32);
	
	sigbuffer_add_value (&buf, 0x06);
	encode_custom_modifiers (assembly, fb->modreq, fb->modopt, &buf);
	/* encode custom attributes before the type */
	encode_reflection_type (assembly, (MonoReflectionType*)fb->type, &buf);
	idx = sigbuffer_add_to_blob_cached (assembly, &buf);
	sigbuffer_free (&buf);
	return idx;
}