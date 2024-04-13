static int __find_bmc_guid(struct device *dev, const void *data)
{
	const guid_t *guid = data;
	struct bmc_device *bmc;
	int rv;

	if (dev->type != &bmc_device_type)
		return 0;

	bmc = to_bmc_device(dev);
	rv = bmc->dyn_guid_set && guid_equal(&bmc->guid, guid);
	if (rv)
		rv = kref_get_unless_zero(&bmc->usecount);
	return rv;
}