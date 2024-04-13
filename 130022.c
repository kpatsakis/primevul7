mono_method_clear_object (MonoDomain *domain, MonoMethod *method)
{
	MonoClass *klass;
	g_assert (method->dynamic);

	klass = method->klass;
	while (klass) {
		clear_cached_object (domain, method, klass);
		klass = klass->parent;
	}
	/* Added by mono_param_get_objects () */
	clear_cached_object (domain, &(method->signature), NULL);
	klass = method->klass;
	while (klass) {
		clear_cached_object (domain, &(method->signature), klass);
		klass = klass->parent;
	}
}