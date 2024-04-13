check_overflow (VerifyContext *ctx)
{
	if (ctx->eval.size >= ctx->max_stack) {
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Method doesn't have stack-depth %d at 0x%04x", ctx->eval.size + 1, ctx->ip_offset));
		return 0;
	}
	return 1;
}