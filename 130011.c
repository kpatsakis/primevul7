do_throw (VerifyContext *ctx)
{
	ILStackDesc *exception;
	if (!check_underflow (ctx, 1))
		return;
	exception = stack_pop (ctx);

	if (!stack_slot_is_null_literal (exception) && !(stack_slot_get_type (exception) == TYPE_COMPLEX && !mono_class_from_mono_type (exception->type)->valuetype))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid type on stack for throw, expected reference type at 0x%04x", ctx->ip_offset));

	if (mono_type_is_generic_argument (exception->type) && !stack_slot_is_boxed_value (exception)) {
		char *name = mono_type_full_name (exception->type);
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid type on stack for throw, expected reference type but found unboxed %s  at 0x%04x ", name, ctx->ip_offset));
		g_free (name);
	}
	/*The stack is left empty after a throw*/
	ctx->eval.size = 0;
}