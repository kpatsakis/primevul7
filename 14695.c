flush_running(struct pxa3xx_gcu_priv *priv)
{
	struct pxa3xx_gcu_batch *running = priv->running;
	struct pxa3xx_gcu_batch *next;

	while (running) {
		next = running->next;
		running->next = priv->free;
		priv->free = running;
		running = next;
	}

	priv->running = NULL;
}