get_dbnull (MonoDomain *domain, MonoObject **dbnull)
{
	if (!*dbnull)
		*dbnull = mono_get_dbnull_object (domain);
	return *dbnull;
}