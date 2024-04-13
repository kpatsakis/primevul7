static gboolean
is_sre_ctor_builder (MonoClass *class)
{
	check_corlib_type_cached (class, "System.Reflection.Emit", "ConstructorBuilder");