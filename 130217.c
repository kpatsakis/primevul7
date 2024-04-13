get_reflection_missing (MonoDomain *domain, MonoObject **reflection_missing)
{
	if (!*reflection_missing)
		*reflection_missing = mono_get_reflection_missing_object (domain);
	return *reflection_missing;
}