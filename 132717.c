static ssize_t aux_firmware_rev_show(struct device *dev,
				     struct device_attribute *attr,
				     char *buf)
{
	struct bmc_device *bmc = to_bmc_device(dev);
	struct ipmi_device_id id;
	int rv;

	rv = bmc_get_device_id(NULL, bmc, &id, NULL, NULL);
	if (rv)
		return rv;

	return snprintf(buf, 21, "0x%02x 0x%02x 0x%02x 0x%02x\n",
			id.aux_firmware_revision[3],
			id.aux_firmware_revision[2],
			id.aux_firmware_revision[1],
			id.aux_firmware_revision[0]);
}