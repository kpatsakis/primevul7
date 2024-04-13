do_push_static_field (VerifyContext *ctx, int token, gboolean take_addr)
{
	MonoClassField *field;
	MonoClass *klass;
	if (!check_overflow (ctx))
		return;
	if (!take_addr)
		CLEAR_PREFIX (ctx, PREFIX_VOLATILE);

	if (!(field = verifier_load_field (ctx, token, &klass, take_addr ? "ldsflda" : "ldsfld")))
		return;

	if (!(field->type->attrs & FIELD_ATTRIBUTE_STATIC)) { 
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Cannot load non static field at 0x%04x", ctx->ip_offset));
		return;
	}
	/*taking the address of initonly field only works from the static constructor */
	if (take_addr && (field->type->attrs & FIELD_ATTRIBUTE_INIT_ONLY) &&
		!(field->parent == ctx->method->klass && (ctx->method->flags & (METHOD_ATTRIBUTE_SPECIAL_NAME | METHOD_ATTRIBUTE_STATIC)) && !strcmp (".cctor", ctx->method->name)))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot take the address of a init-only field at 0x%04x", ctx->ip_offset));

	if (!IS_SKIP_VISIBILITY (ctx) && !mono_method_can_access_field_full (ctx->method, field, NULL))
		CODE_NOT_VERIFIABLE2 (ctx, g_strdup_printf ("Type at stack is not accessible at 0x%04x", ctx->ip_offset), MONO_EXCEPTION_FIELD_ACCESS);

	set_stack_value (ctx, stack_push (ctx), field->type, take_addr);
}