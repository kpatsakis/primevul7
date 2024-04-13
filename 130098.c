do_mkrefany (VerifyContext *ctx, int token)
{
	ILStackDesc *top;
	MonoType *type;
	if (!check_underflow (ctx, 1))
		return;

	if (!(type = get_boxable_mono_type (ctx, token, "refanyval")))
		return;

	top = stack_pop (ctx);

	if (stack_slot_is_managed_mutability_pointer (top))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot use a readonly pointer with mkrefany at 0x%04x", ctx->ip_offset));

	if (!stack_slot_is_managed_pointer (top)) {
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Expected a managed pointer for mkrefany, but found %s at 0x%04x", stack_slot_get_name (top), ctx->ip_offset));
	}else {
		MonoType *stack_type = mono_type_get_type_byval (top->type);
		if (MONO_TYPE_IS_REFERENCE (type) && !mono_metadata_type_equal (type, stack_type))
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Type not compatible for mkrefany at 0x%04x", ctx->ip_offset));
			
		if (!MONO_TYPE_IS_REFERENCE (type) && !verify_type_compatibility_full (ctx, type, stack_type, TRUE))
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Type not compatible for mkrefany at 0x%04x", ctx->ip_offset));
	}

	set_stack_value (ctx, stack_push (ctx), &mono_defaults.typed_reference_class->byval_arg, FALSE);
}