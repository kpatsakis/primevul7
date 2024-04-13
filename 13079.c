static int aspeed_lpc_ctrl_remove(struct platform_device *pdev)
{
	struct aspeed_lpc_ctrl *lpc_ctrl = dev_get_drvdata(&pdev->dev);

	misc_deregister(&lpc_ctrl->miscdev);
	clk_disable_unprepare(lpc_ctrl->clk);

	return 0;
}