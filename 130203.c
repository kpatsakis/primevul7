mono_property_get_object (MonoDomain *domain, MonoClass *klass, MonoProperty *property)
{
	MonoReflectionProperty *res;
	static MonoClass *monoproperty_klass;

	CHECK_OBJECT (MonoReflectionProperty *, property, klass);
	if (!monoproperty_klass)
		monoproperty_klass = mono_class_from_name (mono_defaults.corlib, "System.Reflection", "MonoProperty");
	res = (MonoReflectionProperty *)mono_object_new (domain, monoproperty_klass);
	res->klass = klass;
	res->property = property;
	CACHE_OBJECT (MonoReflectionProperty *, property, res, klass);
}