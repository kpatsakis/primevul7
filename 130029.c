mono_reflection_type_get_underlying_system_type (MonoReflectionType* t)
{
	static MonoMethod *method_get_underlying_system_type = NULL;
	MonoMethod *usertype_method;

	if (!method_get_underlying_system_type)
		method_get_underlying_system_type = mono_class_get_method_from_name (mono_defaults.systemtype_class, "get_UnderlyingSystemType", 0);
	usertype_method = mono_object_get_virtual_method ((MonoObject *) t, method_get_underlying_system_type);
        return (MonoReflectionType *) mono_runtime_invoke (usertype_method, t, NULL, NULL);
}