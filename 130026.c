register_module (MonoDomain *domain, MonoReflectionModuleBuilder *res, MonoDynamicImage *module)
{
	CACHE_OBJECT (MonoReflectionModuleBuilder *, module, res, NULL);
}