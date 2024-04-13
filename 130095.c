verify_stack_type_compatibility_full (VerifyContext *ctx, MonoType *type, ILStackDesc *stack, gboolean drop_byref, gboolean valuetype_must_be_boxed)
{
	MonoType *candidate = mono_type_from_stack_slot (stack);
	if (MONO_TYPE_IS_REFERENCE (type) && !type->byref && stack_slot_is_null_literal (stack))
		return TRUE;

	if (is_compatible_boxed_valuetype (ctx, type, candidate, stack, TRUE))
		return TRUE;

	if (valuetype_must_be_boxed && !stack_slot_is_boxed_value (stack) && !MONO_TYPE_IS_REFERENCE (candidate))
		return FALSE;

	if (!valuetype_must_be_boxed && stack_slot_is_boxed_value (stack))
		return FALSE;

	if (drop_byref)
		return verify_type_compatibility_full (ctx, type, mono_type_get_type_byval (candidate), FALSE);

	return verify_type_compatibility_full (ctx, type, candidate, FALSE);
}