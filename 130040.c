static gboolean
is_sr_mono_field (MonoClass *class)
{
	check_corlib_type_cached (class, "System.Reflection", "MonoField");