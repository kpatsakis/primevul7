stack_pop_safe (VerifyContext *ctx)
{
	g_assert (ctx->eval.size > 0);
	return ctx->eval.stack + --ctx->eval.size;
}