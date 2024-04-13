static gboolean
is_sre_field_builder (MonoClass *class)
{
	check_corlib_type_cached (class, "System.Reflection.Emit", "FieldBuilder");