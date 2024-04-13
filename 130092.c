stack_push_stack_val (VerifyContext *ctx, ILStackDesc *value)
{
	ILStackDesc *top = stack_push (ctx);
	copy_stack_value (top, value);
	return top;
}