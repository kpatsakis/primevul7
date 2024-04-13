do_load_token (VerifyContext *ctx, int token) 
{
	gpointer handle;
	MonoClass *handle_class;
	if (!check_overflow (ctx))
		return;

	switch (token & 0xff000000) {
	case MONO_TOKEN_TYPE_DEF:
	case MONO_TOKEN_TYPE_REF:
	case MONO_TOKEN_TYPE_SPEC:
	case MONO_TOKEN_FIELD_DEF:
	case MONO_TOKEN_METHOD_DEF:
	case MONO_TOKEN_METHOD_SPEC:
	case MONO_TOKEN_MEMBER_REF:
		if (!token_bounds_check (ctx->image, token)) {
			ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Table index out of range 0x%x for token %x for ldtoken at 0x%04x", mono_metadata_token_index (token), token, ctx->ip_offset));
			return;
		}
		break;
	default:
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Invalid table 0x%x for token 0x%x for ldtoken at 0x%04x", mono_metadata_token_table (token), token, ctx->ip_offset));
		return;
	}

	handle = mono_ldtoken (ctx->image, token, &handle_class, ctx->generic_context);
	if (!handle) {
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Invalid token 0x%x for ldtoken at 0x%04x", token, ctx->ip_offset));
		return;
	}
	if (handle_class == mono_defaults.typehandle_class) {
		mono_type_is_valid_in_context (ctx, (MonoType*)handle);
	} else if (handle_class == mono_defaults.methodhandle_class) {
		mono_method_is_valid_in_context (ctx, (MonoMethod*)handle);		
	} else if (handle_class == mono_defaults.fieldhandle_class) {
		mono_type_is_valid_in_context (ctx, &((MonoClassField*)handle)->parent->byval_arg);				
	} else {
		ADD_VERIFY_ERROR2 (ctx, g_strdup_printf ("Invalid ldtoken type %x at 0x%04x", token, ctx->ip_offset), MONO_EXCEPTION_BAD_IMAGE);
	}
	stack_push_val (ctx, TYPE_COMPLEX, mono_class_get_type (handle_class));
}