static MonoMethodSignature*
method_builder_to_signature (MonoImage *image, MonoReflectionMethodBuilder *method) {
	MonoMethodSignature *sig;

	sig = parameters_to_signature (image, method->parameters);
	sig->hasthis = method->attrs & METHOD_ATTRIBUTE_STATIC? 0: 1;
	sig->ret = method->rtype? mono_reflection_type_get_handle ((MonoReflectionType*)method->rtype): &mono_defaults.void_class->byval_arg;
	sig->generic_param_count = method->generic_params ? mono_array_length (method->generic_params) : 0;
	return sig;