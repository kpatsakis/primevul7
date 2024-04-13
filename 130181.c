static void
ensure_generic_class_runtime_vtable (MonoClass *klass)
{
	MonoClass *gklass = klass->generic_class->container_class;

	ensure_runtime_vtable (gklass);	

	fix_partial_generic_class (klass);