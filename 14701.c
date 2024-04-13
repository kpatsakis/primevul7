dump_whole_state(struct pxa3xx_gcu_priv *priv)
{
	struct pxa3xx_gcu_shared *sh = priv->shared;
	u32 base = gc_readl(priv, REG_GCRBBR);

	QDUMP("DUMP");

	printk(KERN_DEBUG "== PXA3XX-GCU DUMP ==\n"
		"%s, STATUS 0x%02lx, B 0x%08lx [%ld], E %5ld, H %5ld, T %5ld\n",
		sh->hw_running ? "running" : "idle   ",
		gc_readl(priv, REG_GCISCR),
		gc_readl(priv, REG_GCRBBR),
		gc_readl(priv, REG_GCRBLR),
		(gc_readl(priv, REG_GCRBEXHR) - base) / 4,
		(gc_readl(priv, REG_GCRBHR) - base) / 4,
		(gc_readl(priv, REG_GCRBTR) - base) / 4);
}