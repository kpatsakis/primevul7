mono_get_reflection_missing_object (MonoDomain *domain)
{
	MonoObject *obj;
	static MonoClassField *missing_value_field = NULL;
	
	if (!missing_value_field) {
		MonoClass *missing_klass;
		missing_klass = mono_class_from_name (mono_defaults.corlib, "System.Reflection", "Missing");
		mono_class_init (missing_klass);
		missing_value_field = mono_class_get_field_from_name (missing_klass, "Value");
		g_assert (missing_value_field);
	}
	obj = mono_field_get_value_object (domain, missing_value_field, NULL); 
	g_assert (obj);
	return obj;
}