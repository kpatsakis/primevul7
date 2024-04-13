do_endfilter (VerifyContext *ctx)
{
	MonoExceptionClause *clause;

	if (IS_STRICT_MODE (ctx)) {
		if (ctx->eval.size != 1)
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Stack size must have one item for endfilter at 0x%04x", ctx->ip_offset));

		if (ctx->eval.size >= 1 && stack_slot_get_type (stack_pop (ctx)) != TYPE_I4)
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Stack item type is not an int32 for endfilter at 0x%04x", ctx->ip_offset));
	}

	if ((clause = is_correct_endfilter (ctx, ctx->ip_offset))) {
		if (IS_STRICT_MODE (ctx)) {
			if (ctx->ip_offset != clause->handler_offset - 2)
				ADD_VERIFY_ERROR (ctx, g_strdup_printf ("endfilter is not the last instruction of the filter clause at 0x%04x", ctx->ip_offset));			
		} else {
			if ((ctx->ip_offset != clause->handler_offset - 2) && !MONO_OFFSET_IN_HANDLER (clause, ctx->ip_offset))
				ADD_VERIFY_ERROR (ctx, g_strdup_printf ("endfilter is not the last instruction of the filter clause at 0x%04x", ctx->ip_offset));
		}
	} else {
		if (IS_STRICT_MODE (ctx) && !is_unverifiable_endfilter (ctx, ctx->ip_offset))
			ADD_VERIFY_ERROR (ctx, g_strdup_printf ("endfilter outside filter clause at 0x%04x", ctx->ip_offset));
		else
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("endfilter outside filter clause at 0x%04x", ctx->ip_offset));
	}

	ctx->eval.size = 0;
}