create_cattr_typed_arg (MonoType *t, MonoObject *val)
{
	static MonoClass *klass;
	static MonoMethod *ctor;
	MonoObject *retval;
	void *params [2], *unboxed;

	if (!klass)
		klass = mono_class_from_name (mono_defaults.corlib, "System.Reflection", "CustomAttributeTypedArgument");
	if (!ctor)
		ctor = mono_class_get_method_from_name (klass, ".ctor", 2);
	
	params [0] = mono_type_get_object (mono_domain_get (), t);
	params [1] = val;
	retval = mono_object_new (mono_domain_get (), klass);
	unboxed = mono_object_unbox (retval);
	mono_runtime_invoke (ctor, unboxed, params, NULL);

	return retval;
}