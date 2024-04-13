static int pxa3xx_gcu_remove(struct platform_device *pdev)
{
	struct pxa3xx_gcu_priv *priv = platform_get_drvdata(pdev);
	struct device *dev = &pdev->dev;

	pxa3xx_gcu_wait_idle(priv);
	misc_deregister(&priv->misc_dev);
	dma_free_coherent(dev, SHARED_SIZE, priv->shared, priv->shared_phys);
	clk_disable_unprepare(priv->clk);
	pxa3xx_gcu_free_buffers(dev, priv);

	return 0;
}