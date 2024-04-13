store_arg (VerifyContext *ctx, guint32 arg)
{
	ILStackDesc *value;

	if (arg >= ctx->max_args) {
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Method doesn't have argument %d at 0x%04x", arg + 1, ctx->ip_offset));
		if (check_underflow (ctx, 1))
			stack_pop (ctx);
		return;
	}

	if (check_underflow (ctx, 1)) {
		value = stack_pop (ctx);
		if (!verify_stack_type_compatibility (ctx, ctx->params [arg], value)) {
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Incompatible type %s in argument store at 0x%04x", stack_slot_get_name (value), ctx->ip_offset));
		}
	}
	if (arg == 0 && !(ctx->method->flags & METHOD_ATTRIBUTE_STATIC))
		ctx->has_this_store = 1;
}