is_valid_type_in_context (VerifyContext *ctx, MonoType *type)
{
	return mono_type_is_valid_type_in_context (type, ctx->generic_context);
}