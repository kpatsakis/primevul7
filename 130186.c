fieldref_encode_signature (MonoDynamicImage *assembly, MonoImage *field_image, MonoType *type)
{
	SigBuffer buf;
	guint32 idx, i, token;

	if (!assembly->save)
		return 0;

	sigbuffer_init (&buf, 32);
	
	sigbuffer_add_value (&buf, 0x06);
	/* encode custom attributes before the type */
	if (type->num_mods) {
		for (i = 0; i < type->num_mods; ++i) {
			if (field_image) {
				MonoClass *class = mono_class_get (field_image, type->modifiers [i].token);
				g_assert (class);
				token = mono_image_typedef_or_ref (assembly, &class->byval_arg);
			} else {
				token = type->modifiers [i].token;
			}

			if (type->modifiers [i].required)
				sigbuffer_add_byte (&buf, MONO_TYPE_CMOD_REQD);
			else
				sigbuffer_add_byte (&buf, MONO_TYPE_CMOD_OPT);

			sigbuffer_add_value (&buf, token);
		}
	}
	encode_type (assembly, type, &buf);
	idx = sigbuffer_add_to_blob_cached (assembly, &buf);
	sigbuffer_free (&buf);
	return idx;
}