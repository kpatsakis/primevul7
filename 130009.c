do_switch (VerifyContext *ctx, int count, const unsigned char *data)
{
	int i, base = ctx->ip_offset + 5 + count * 4;
	ILStackDesc *value;

	if (!check_underflow (ctx, 1))
		return;

	value = stack_pop (ctx);

	if (stack_slot_get_type (value) != TYPE_I4 && stack_slot_get_type (value) != TYPE_NATIVE_INT)
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid argument to switch at 0x%04x", ctx->ip_offset));

	for (i = 0; i < count; ++i) {
		int target = base + read32 (data + i * 4);

		if (target < 0 || target >= ctx->code_size) {
			ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Switch target %x out of code at 0x%04x", i, ctx->ip_offset));
			return;
		}

		switch (is_valid_branch_instruction (ctx->header, ctx->ip_offset, target)) {
		case 1:
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Switch target %x escapes out of exception block at 0x%04x", i, ctx->ip_offset));
			break;
		case 2:
			ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Switch target %x escapes out of exception block at 0x%04x", i, ctx->ip_offset));
			return;
		}
		merge_stacks (ctx, &ctx->eval, &ctx->code [target], FALSE, TRUE);
	}
}