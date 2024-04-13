int snd_sof_dbg_init(struct snd_sof_dev *sdev)
{
	const struct snd_sof_dsp_ops *ops = sof_ops(sdev);
	const struct snd_sof_debugfs_map *map;
	int i;
	int err;

	/* use "sof" as top level debugFS dir */
	sdev->debugfs_root = debugfs_create_dir("sof", NULL);

	/* init dfsentry list */
	INIT_LIST_HEAD(&sdev->dfsentry_list);

	/* create debugFS files for platform specific MMIO/DSP memories */
	for (i = 0; i < ops->debug_map_count; i++) {
		map = &ops->debug_map[i];

		err = snd_sof_debugfs_io_item(sdev, sdev->bar[map->bar] +
					      map->offset, map->size,
					      map->name, map->access_type);
		/* errors are only due to memory allocation, not debugfs */
		if (err < 0)
			return err;
	}

#if IS_ENABLED(CONFIG_SND_SOC_SOF_DEBUG_IPC_FLOOD_TEST)
	/* create read-write ipc_flood_count debugfs entry */
	err = snd_sof_debugfs_buf_item(sdev, NULL, 0,
				       "ipc_flood_count", 0666);

	/* errors are only due to memory allocation, not debugfs */
	if (err < 0)
		return err;

	/* create read-write ipc_flood_duration_ms debugfs entry */
	err = snd_sof_debugfs_buf_item(sdev, NULL, 0,
				       "ipc_flood_duration_ms", 0666);

	/* errors are only due to memory allocation, not debugfs */
	if (err < 0)
		return err;
#endif

	return 0;
}