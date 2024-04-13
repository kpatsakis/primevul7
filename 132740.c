static struct bmc_device *ipmi_find_bmc_prod_dev_id(
	struct device_driver *drv,
	unsigned int product_id, unsigned char device_id)
{
	struct prod_dev_id id = {
		.product_id = product_id,
		.device_id = device_id,
	};
	struct device *dev;
	struct bmc_device *bmc = NULL;

	dev = driver_find_device(drv, NULL, &id, __find_bmc_prod_dev_id);
	if (dev) {
		bmc = to_bmc_device(dev);
		put_device(dev);
	}
	return bmc;
}