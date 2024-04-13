check_unverifiable_type (VerifyContext *ctx, MonoType *type)
{
	if (type->type == MONO_TYPE_PTR || type->type == MONO_TYPE_FNPTR) {
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Unmanaged pointer is not a verifiable type at 0x%04x", ctx->ip_offset));
		return 0;
	}
	return 1;
}