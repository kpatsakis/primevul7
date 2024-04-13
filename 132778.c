static void __ipmi_bmc_unregister(struct ipmi_smi *intf)
{
	struct bmc_device *bmc = intf->bmc;

	if (!intf->bmc_registered)
		return;

	sysfs_remove_link(&intf->si_dev->kobj, "bmc");
	sysfs_remove_link(&bmc->pdev.dev.kobj, intf->my_dev_name);
	kfree(intf->my_dev_name);
	intf->my_dev_name = NULL;

	mutex_lock(&bmc->dyn_mutex);
	list_del(&intf->bmc_link);
	mutex_unlock(&bmc->dyn_mutex);
	intf->bmc = &intf->tmp_bmc;
	kref_put(&bmc->usecount, cleanup_bmc_device);
	intf->bmc_registered = false;
}