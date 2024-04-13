check_unmanaged_pointer (VerifyContext *ctx, ILStackDesc *value)
{
	if (stack_slot_get_type (value) == TYPE_PTR) {
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Unmanaged pointer is not a verifiable type at 0x%04x", ctx->ip_offset));
		return 0;
	}
	return 1;
}