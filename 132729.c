static ssize_t revision_show(struct device *dev, struct device_attribute *attr,
			     char *buf)
{
	struct bmc_device *bmc = to_bmc_device(dev);
	struct ipmi_device_id id;
	int rv;

	rv = bmc_get_device_id(NULL, bmc, &id, NULL, NULL);
	if (rv)
		return rv;

	return snprintf(buf, 20, "%u\n", id.device_revision & 0x0F);
}