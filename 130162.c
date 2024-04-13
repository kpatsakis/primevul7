do_cpobj (VerifyContext *ctx, int token)
{
	ILStackDesc *dest, *src;
	MonoType *type = get_boxable_mono_type (ctx, token, "cpobj");
	if (!type)
		return;

	if (!check_underflow (ctx, 2))
		return;

	src = stack_pop (ctx);
	dest = stack_pop (ctx);

	if (!stack_slot_is_managed_pointer (src)) 
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid source of cpobj operation at 0x%04x", ctx->ip_offset));

	if (!stack_slot_is_managed_pointer (dest)) 
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid destination of cpobj operation at 0x%04x", ctx->ip_offset));

	if (stack_slot_is_managed_mutability_pointer (dest))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot use a readonly pointer with cpobj at 0x%04x", ctx->ip_offset));

	if (!verify_type_compatibility (ctx, type, mono_type_get_type_byval (src->type)))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Token and source types of cpobj don't match at 0x%04x", ctx->ip_offset));

	if (!verify_type_compatibility (ctx, mono_type_get_type_byval (dest->type), type))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Destination and token types of cpobj don't match at 0x%04x", ctx->ip_offset));
}