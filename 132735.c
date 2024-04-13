static ssize_t guid_show(struct device *dev, struct device_attribute *attr,
			 char *buf)
{
	struct bmc_device *bmc = to_bmc_device(dev);
	bool guid_set;
	guid_t guid;
	int rv;

	rv = bmc_get_device_id(NULL, bmc, NULL, &guid_set, &guid);
	if (rv)
		return rv;
	if (!guid_set)
		return -ENOENT;

	return snprintf(buf, UUID_STRING_LEN + 1 + 1, "%pUl\n", &guid);
}