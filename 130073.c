static gboolean
is_sre_generic_instance (MonoClass *class)
{
	check_corlib_type_cached (class, "System.Reflection", "MonoGenericClass");