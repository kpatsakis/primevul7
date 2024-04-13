static void pxa3xx_gcu_debug_timedout(struct timer_list *unused)
{
	struct pxa3xx_gcu_priv *priv = debug_timer_priv;

	QERROR("Timer DUMP");

	mod_timer(&pxa3xx_gcu_debug_timer, jiffies + 5 * HZ);
}