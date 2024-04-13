static ssize_t product_id_show(struct device *dev,
			       struct device_attribute *attr,
			       char *buf)
{
	struct bmc_device *bmc = to_bmc_device(dev);
	struct ipmi_device_id id;
	int rv;

	rv = bmc_get_device_id(NULL, bmc, &id, NULL, NULL);
	if (rv)
		return rv;

	return snprintf(buf, 10, "0x%4.4x\n", id.product_id);
}