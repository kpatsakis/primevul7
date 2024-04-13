stack_slot_full_name (ILStackDesc *value)
{
	char *type_name = mono_type_full_name (value->type);
	char *stack_name = stack_slot_stack_type_full_name (value);
	char *res = g_strdup_printf ("%s (%s)", type_name, stack_name);
	g_free (type_name);
	g_free (stack_name);
	return res;
}