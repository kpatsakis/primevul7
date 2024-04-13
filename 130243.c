static MonoReflectionType*
mono_reflection_type_resolve_user_types (MonoReflectionType *type)
{
	if (!type || type->type)
		return type;

	if (is_usertype (type)) {
		type = mono_reflection_type_get_underlying_system_type (type);
		if (is_usertype (type))
			mono_raise_exception (mono_get_exception_not_supported ("User defined subclasses of System.Type are not yet supported22"));
	}

	return type;