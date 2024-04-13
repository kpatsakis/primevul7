verify_type_compatibility (VerifyContext *ctx, MonoType *target, MonoType *candidate)
{
	return verify_type_compatibility_full (ctx, target, candidate, FALSE);
}