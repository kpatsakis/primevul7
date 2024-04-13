static void pxa3xx_gcu_init_debug_timer(struct pxa3xx_gcu_priv *priv)
{
	/* init the timer structure */
	debug_timer_priv = priv;
	timer_setup(&pxa3xx_gcu_debug_timer, pxa3xx_gcu_debug_timedout, 0);
	pxa3xx_gcu_debug_timedout(NULL);
}