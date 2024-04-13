static gboolean
is_sre_ctor_on_tb_inst (MonoClass *class)
{
	check_corlib_type_cached (class, "System.Reflection.Emit", "ConstructorOnTypeBuilderInst");