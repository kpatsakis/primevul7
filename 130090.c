stack_push (VerifyContext *ctx)
{
	g_assert (ctx->eval.size < ctx->max_stack);
	g_assert (ctx->eval.size <= ctx->eval.max_size);

	ensure_stack_size (&ctx->eval, ctx->eval.size + 1);

	return & ctx->eval.stack [ctx->eval.size++];
}