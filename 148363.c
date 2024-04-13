static int __init redirect_tg_init(void)
{
	return xt_register_targets(redirect_tg_reg,
				   ARRAY_SIZE(redirect_tg_reg));
}