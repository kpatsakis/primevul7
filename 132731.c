static int __find_bmc_prod_dev_id(struct device *dev, const void *data)
{
	const struct prod_dev_id *cid = data;
	struct bmc_device *bmc;
	int rv;

	if (dev->type != &bmc_device_type)
		return 0;

	bmc = to_bmc_device(dev);
	rv = (bmc->id.product_id == cid->product_id
	      && bmc->id.device_id == cid->device_id);
	if (rv)
		rv = kref_get_unless_zero(&bmc->usecount);
	return rv;
}