static gboolean
is_sr_mono_cmethod (MonoClass *class)
{
	check_corlib_type_cached (class, "System.Reflection", "MonoCMethod");