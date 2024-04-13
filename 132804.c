static struct bmc_device *ipmi_find_bmc_guid(struct device_driver *drv,
					     guid_t *guid)
{
	struct device *dev;
	struct bmc_device *bmc = NULL;

	dev = driver_find_device(drv, NULL, guid, __find_bmc_guid);
	if (dev) {
		bmc = to_bmc_device(dev);
		put_device(dev);
	}
	return bmc;
}