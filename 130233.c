do_cast (VerifyContext *ctx, int token, const char *opcode) {
	ILStackDesc *value;
	MonoType *type;
	gboolean is_boxed;
	gboolean do_box;

	if (!check_underflow (ctx, 1))
		return;

	if (!(type = verifier_load_type (ctx, token, opcode)))
		return;

	if (type->byref) {
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Invalid %s type at 0x%04x", opcode, ctx->ip_offset));
		return;
	}

	value = stack_pop (ctx);
	is_boxed = stack_slot_is_boxed_value (value);

	if (stack_slot_is_managed_pointer (value))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid value for %s at 0x%04x", opcode, ctx->ip_offset));
	else if (!MONO_TYPE_IS_REFERENCE  (value->type) && !is_boxed) {
		char *name = stack_slot_full_name (value);
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Expected a reference type on stack for %s but found %s at 0x%04x", opcode, name, ctx->ip_offset));
		g_free (name);
	}

	switch (value->type->type) {
	case MONO_TYPE_FNPTR:
	case MONO_TYPE_PTR:
	case MONO_TYPE_TYPEDBYREF: 
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid value for %s at 0x%04x", opcode, ctx->ip_offset));
	}

	do_box = is_boxed || mono_type_is_generic_argument(type) || mono_class_from_mono_type (type)->valuetype;
	stack_push_val (ctx, TYPE_COMPLEX | (do_box ? BOXED_MASK : 0), type);
}