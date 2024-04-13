static umode_t bmc_dev_attr_is_visible(struct kobject *kobj,
				       struct attribute *attr, int idx)
{
	struct device *dev = kobj_to_dev(kobj);
	struct bmc_device *bmc = to_bmc_device(dev);
	umode_t mode = attr->mode;
	int rv;

	if (attr == &dev_attr_aux_firmware_revision.attr) {
		struct ipmi_device_id id;

		rv = bmc_get_device_id(NULL, bmc, &id, NULL, NULL);
		return (!rv && id.aux_firmware_revision_set) ? mode : 0;
	}
	if (attr == &dev_attr_guid.attr) {
		bool guid_set;

		rv = bmc_get_device_id(NULL, bmc, NULL, &guid_set, NULL);
		return (!rv && guid_set) ? mode : 0;
	}
	return mode;
}