do_localloc (VerifyContext *ctx)
{
	ILStackDesc *top;
	
	if (ctx->eval.size != 1) {
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Stack must have only size item in localloc at 0x%04x", ctx->ip_offset));
		return;		
	}

	if (in_any_exception_block (ctx->header, ctx->ip_offset)) {
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Stack must have only size item in localloc at 0x%04x", ctx->ip_offset));
		return;
	}

	/*TODO verify top type*/
	top = stack_pop (ctx);

	set_stack_value (ctx, stack_push (ctx), &mono_defaults.int_class->byval_arg, FALSE);
	CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Instruction localloc in never verifiable at 0x%04x", ctx->ip_offset));
}