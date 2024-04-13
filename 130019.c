static MonoMethodSignature*
parameters_to_signature (MonoImage *image, MonoArray *parameters) {
	MonoMethodSignature *sig;
	int count, i;

	count = parameters? mono_array_length (parameters): 0;

	sig = image_g_malloc0 (image, MONO_SIZEOF_METHOD_SIGNATURE + sizeof (MonoType*) * count);
	sig->param_count = count;
	sig->sentinelpos = -1; /* FIXME */
	for (i = 0; i < count; ++i)
		sig->params [i] = mono_type_array_get_and_resolve (parameters, i);
	return sig;