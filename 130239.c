static gboolean
is_sre_array (MonoClass *class)
{
	check_corlib_type_cached (class, "System.Reflection.Emit", "ArrayType");