mono_method_get_object (MonoDomain *domain, MonoMethod *method, MonoClass *refclass)
{
	/*
	 * We use the same C representation for methods and constructors, but the type 
	 * name in C# is different.
	 */
	static MonoClass *System_Reflection_MonoMethod = NULL;
	static MonoClass *System_Reflection_MonoCMethod = NULL;
	static MonoClass *System_Reflection_MonoGenericMethod = NULL;
	static MonoClass *System_Reflection_MonoGenericCMethod = NULL;
	MonoClass *klass;
	MonoReflectionMethod *ret;

	if (method->is_inflated) {
		MonoReflectionGenericMethod *gret;

		refclass = method->klass;
		CHECK_OBJECT (MonoReflectionMethod *, method, refclass);
		if ((*method->name == '.') && (!strcmp (method->name, ".ctor") || !strcmp (method->name, ".cctor"))) {
			if (!System_Reflection_MonoGenericCMethod)
				System_Reflection_MonoGenericCMethod = mono_class_from_name (mono_defaults.corlib, "System.Reflection", "MonoGenericCMethod");
			klass = System_Reflection_MonoGenericCMethod;
		} else {
			if (!System_Reflection_MonoGenericMethod)
				System_Reflection_MonoGenericMethod = mono_class_from_name (mono_defaults.corlib, "System.Reflection", "MonoGenericMethod");
			klass = System_Reflection_MonoGenericMethod;
		}
		gret = (MonoReflectionGenericMethod*)mono_object_new (domain, klass);
		gret->method.method = method;
		MONO_OBJECT_SETREF (gret, method.name, mono_string_new (domain, method->name));
		MONO_OBJECT_SETREF (gret, method.reftype, mono_type_get_object (domain, &refclass->byval_arg));
		CACHE_OBJECT (MonoReflectionMethod *, method, (MonoReflectionMethod*)gret, refclass);
	}

	if (!refclass)
		refclass = method->klass;

	CHECK_OBJECT (MonoReflectionMethod *, method, refclass);
	if (*method->name == '.' && (strcmp (method->name, ".ctor") == 0 || strcmp (method->name, ".cctor") == 0)) {
		if (!System_Reflection_MonoCMethod)
			System_Reflection_MonoCMethod = mono_class_from_name (mono_defaults.corlib, "System.Reflection", "MonoCMethod");
		klass = System_Reflection_MonoCMethod;
	}
	else {
		if (!System_Reflection_MonoMethod)
			System_Reflection_MonoMethod = mono_class_from_name (mono_defaults.corlib, "System.Reflection", "MonoMethod");
		klass = System_Reflection_MonoMethod;
	}
	ret = (MonoReflectionMethod*)mono_object_new (domain, klass);
	ret->method = method;
	MONO_OBJECT_SETREF (ret, reftype, mono_type_get_object (domain, &refclass->byval_arg));
	CACHE_OBJECT (MonoReflectionMethod *, method, ret, refclass);
}