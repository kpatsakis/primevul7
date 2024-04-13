check_underflow (VerifyContext *ctx, int size)
{
	if (ctx->eval.size < size) {
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Stack underflow, required %d, but have %d at 0x%04x", size, ctx->eval.size, ctx->ip_offset));
		return 0;
	}
	return 1;
}