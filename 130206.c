static MonoMethodSignature*
dynamic_method_to_signature (MonoReflectionDynamicMethod *method) {
	MonoMethodSignature *sig;

	sig = parameters_to_signature (NULL, method->parameters);
	sig->hasthis = method->attrs & METHOD_ATTRIBUTE_STATIC? 0: 1;
	sig->ret = method->rtype? mono_reflection_type_get_handle (method->rtype): &mono_defaults.void_class->byval_arg;
	sig->generic_param_count = 0;
	return sig;