gc_writel(struct pxa3xx_gcu_priv *priv, unsigned int off, unsigned long val)
{
	__raw_writel(val, priv->mmio_base + off);
}