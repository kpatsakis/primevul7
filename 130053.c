do_store_field (VerifyContext *ctx, int token)
{
	ILStackDesc *value, *obj;
	MonoClassField *field;
	CLEAR_PREFIX (ctx, PREFIX_UNALIGNED | PREFIX_VOLATILE);

	if (!check_underflow (ctx, 2))
		return;

	value = stack_pop (ctx);
	obj = stack_pop_safe (ctx);

	if (!check_is_valid_type_for_field_ops (ctx, token, obj, &field, "stfld"))
		return;

	if (!verify_stack_type_compatibility (ctx, field->type, value))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Incompatible type %s in field store at 0x%04x", stack_slot_get_name (value), ctx->ip_offset));	
}