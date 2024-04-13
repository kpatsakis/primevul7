static gboolean
is_sr_mono_generic_method (MonoClass *class)
{
	check_corlib_type_cached (class, "System.Reflection", "MonoGenericMethod");