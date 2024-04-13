do_newarr (VerifyContext *ctx, int token) 
{
	ILStackDesc *value;
	MonoType *type = get_boxable_mono_type (ctx, token, "newarr");

	if (!type)
		return;

	if (!check_underflow (ctx, 1))
		return;

	value = stack_pop (ctx);
	if (stack_slot_get_type (value) != TYPE_I4 && stack_slot_get_type (value) != TYPE_NATIVE_INT)
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Array size type on stack (%s) is not a verifiable type at 0x%04x", stack_slot_get_name (value), ctx->ip_offset));

	set_stack_value (ctx, stack_push (ctx), mono_class_get_type (mono_array_class_get (mono_class_from_mono_type (type), 1)), FALSE);
}