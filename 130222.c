mono_method_is_valid_in_context (VerifyContext *ctx, MonoMethod *method)
{
	if (!mono_type_is_valid_in_context (ctx, &method->klass->byval_arg))
		return RESULT_INVALID;

	if (!method->is_inflated)
		return RESULT_VALID;

	if (!mono_method_is_valid_generic_instantiation (ctx, method)) {
		ADD_VERIFY_ERROR2 (ctx, g_strdup_printf ("Invalid generic method instantiation of method %s.%s::%s at 0x%04x", method->klass->name_space, method->klass->name, method->name, ctx->ip_offset), MONO_EXCEPTION_UNVERIFIABLE_IL);
		return RESULT_INVALID;
	}

	if (!mono_method_repect_method_constraints (ctx, method)) {
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid generic method instantiation of method %s.%s::%s (generic args don't respect target's constraints) at 0x%04x", method->klass->name_space, method->klass->name, method->name, ctx->ip_offset));
		return RESULT_UNVERIFIABLE;
	}
	return RESULT_VALID;
}