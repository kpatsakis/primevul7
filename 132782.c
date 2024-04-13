release_bmc_device(struct device *dev)
{
	kfree(to_bmc_device(dev));
}