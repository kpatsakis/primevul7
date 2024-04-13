int snd_sof_debugfs_io_item(struct snd_sof_dev *sdev,
			    void __iomem *base, size_t size,
			    const char *name,
			    enum sof_debugfs_access_type access_type)
{
	struct snd_sof_dfsentry *dfse;

	if (!sdev)
		return -EINVAL;

	dfse = devm_kzalloc(sdev->dev, sizeof(*dfse), GFP_KERNEL);
	if (!dfse)
		return -ENOMEM;

	dfse->type = SOF_DFSENTRY_TYPE_IOMEM;
	dfse->io_mem = base;
	dfse->size = size;
	dfse->sdev = sdev;
	dfse->access_type = access_type;

#if IS_ENABLED(CONFIG_SND_SOC_SOF_DEBUG_ENABLE_DEBUGFS_CACHE)
	/*
	 * allocate cache buffer that will be used to save the mem window
	 * contents prior to suspend
	 */
	if (access_type == SOF_DEBUGFS_ACCESS_D0_ONLY) {
		dfse->cache_buf = devm_kzalloc(sdev->dev, size, GFP_KERNEL);
		if (!dfse->cache_buf)
			return -ENOMEM;
	}
#endif

	debugfs_create_file(name, 0444, sdev->debugfs_root, dfse,
			    &sof_dfs_fops);

	/* add to dfsentry list */
	list_add(&dfse->list, &sdev->dfsentry_list);

	return 0;
}