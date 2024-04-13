pxa3xx_gcu_free_buffers(struct device *dev,
			struct pxa3xx_gcu_priv *priv)
{
	struct pxa3xx_gcu_batch *next, *buffer = priv->free;

	while (buffer) {
		next = buffer->next;

		dma_free_coherent(dev, PXA3XX_GCU_BATCH_WORDS * 4,
				  buffer->ptr, buffer->phys);

		kfree(buffer);
		buffer = next;
	}

	priv->free = NULL;
}