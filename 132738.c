static void cleanup_bmc_work(struct work_struct *work)
{
	struct bmc_device *bmc = container_of(work, struct bmc_device,
					      remove_work);
	int id = bmc->pdev.id; /* Unregister overwrites id */

	platform_device_unregister(&bmc->pdev);
	ida_simple_remove(&ipmi_bmc_ida, id);
}