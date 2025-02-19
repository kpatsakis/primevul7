MonoArray *
mono_reflection_sighelper_get_signature_field (MonoReflectionSigHelper *sig)
{
	MonoDynamicImage *assembly = sig->module->dynamic_image;
	guint32 na = sig->arguments ? mono_array_length (sig->arguments) : 0;
	guint32 buflen, i;
	MonoArray *result;
	SigBuffer buf;

	check_array_for_usertypes (sig->arguments);

	sigbuffer_init (&buf, 32);

	sigbuffer_add_value (&buf, 0x06);
	for (i = 0; i < na; ++i) {
		MonoReflectionType *type = mono_array_get (sig->arguments, MonoReflectionType*, i);
		encode_reflection_type (assembly, type, &buf);
	}

	buflen = buf.p - buf.buf;
	result = mono_array_new (mono_domain_get (), mono_defaults.byte_class, buflen);
	memcpy (mono_array_addr (result, char, 0), buf.buf, buflen);
	sigbuffer_free (&buf);

	return result;