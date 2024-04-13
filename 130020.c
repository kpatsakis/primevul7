static gboolean
is_sre_byref (MonoClass *class)
{
	check_corlib_type_cached (class, "System.Reflection.Emit", "ByRefType");