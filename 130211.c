do_box_value (VerifyContext *ctx, int klass_token)
{
	ILStackDesc *value;
	MonoType *type = get_boxable_mono_type (ctx, klass_token, "box");
	MonoClass *klass;	

	if (!type)
		return;

	if (!check_underflow (ctx, 1))
		return;

	value = stack_pop (ctx);
	/*box is a nop for reference types*/

	if (stack_slot_get_underlying_type (value) == TYPE_COMPLEX && MONO_TYPE_IS_REFERENCE (value->type) && MONO_TYPE_IS_REFERENCE (type)) {
		stack_push_stack_val (ctx, value)->stype |= BOXED_MASK;
		return;
	}


	if (!verify_stack_type_compatibility (ctx, type, value))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid type at stack for boxing operation at 0x%04x", ctx->ip_offset));

	klass = mono_class_from_mono_type (type);
	if (mono_class_is_nullable (klass))
		type = &mono_class_get_nullable_param (klass)->byval_arg;
	stack_push_val (ctx, TYPE_COMPLEX | BOXED_MASK, type);
}