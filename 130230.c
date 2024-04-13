do_unary_math_op (VerifyContext *ctx, int op)
{
	ILStackDesc *value;
	if (!check_underflow (ctx, 1))
		return;
	value = stack_pop (ctx);
	switch (stack_slot_get_type (value)) {
	case TYPE_I4:
	case TYPE_I8:
	case TYPE_NATIVE_INT:
		break;
	case TYPE_R8:
		if (op == CEE_NEG)
			break;
	case TYPE_COMPLEX: /*only enums are ok*/
		if (mono_type_is_enum_type (value->type))
			break;
	default:
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid type at stack for unary not at 0x%04x", ctx->ip_offset));
	}
	stack_push_stack_val (ctx, value);
}