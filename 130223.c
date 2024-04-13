encode_custom_modifiers (MonoDynamicImage *assembly, MonoArray *modreq, MonoArray *modopt, SigBuffer *buf)
{
	int i;

	if (modreq) {
		for (i = 0; i < mono_array_length (modreq); ++i) {
			MonoType *mod = mono_type_array_get_and_resolve (modreq, i);
			sigbuffer_add_byte (buf, MONO_TYPE_CMOD_REQD);
			sigbuffer_add_value (buf, mono_image_typedef_or_ref (assembly, mod));
		}
	}
	if (modopt) {
		for (i = 0; i < mono_array_length (modopt); ++i) {
			MonoType *mod = mono_type_array_get_and_resolve (modopt, i);
			sigbuffer_add_byte (buf, MONO_TYPE_CMOD_OPT);
			sigbuffer_add_value (buf, mono_image_typedef_or_ref (assembly, mod));
		}
	}
}