do_refanyval (VerifyContext *ctx, int token)
{
	ILStackDesc *top;
	MonoType *type;
	if (!check_underflow (ctx, 1))
		return;

	if (!(type = get_boxable_mono_type (ctx, token, "refanyval")))
		return;

	top = stack_pop (ctx);

	if (top->stype != TYPE_PTR || top->type->type != MONO_TYPE_TYPEDBYREF)
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Expected a typedref as argument for refanyval, but found %s at 0x%04x", stack_slot_get_name (top), ctx->ip_offset));

	set_stack_value (ctx, stack_push (ctx), type, TRUE);
}