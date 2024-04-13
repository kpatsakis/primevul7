static ssize_t sof_dfsentry_read(struct file *file, char __user *buffer,
				 size_t count, loff_t *ppos)
{
	struct snd_sof_dfsentry *dfse = file->private_data;
	struct snd_sof_dev *sdev = dfse->sdev;
	loff_t pos = *ppos;
	size_t size_ret;
	int skip = 0;
	int size;
	u8 *buf;

#if IS_ENABLED(CONFIG_SND_SOC_SOF_DEBUG_IPC_FLOOD_TEST)
	struct dentry *dentry;

	dentry = file->f_path.dentry;
	if ((!strcmp(dentry->d_name.name, "ipc_flood_count") ||
	     !strcmp(dentry->d_name.name, "ipc_flood_duration_ms")) &&
	    dfse->cache_buf) {
		if (*ppos)
			return 0;

		count = strlen(dfse->cache_buf);
		size_ret = copy_to_user(buffer, dfse->cache_buf, count);
		if (size_ret)
			return -EFAULT;

		*ppos += count;
		return count;
	}
#endif
	size = dfse->size;

	/* validate position & count */
	if (pos < 0)
		return -EINVAL;
	if (pos >= size || !count)
		return 0;
	/* find the minimum. min() is not used since it adds sparse warnings */
	if (count > size - pos)
		count = size - pos;

	/* align io read start to u32 multiple */
	pos = ALIGN_DOWN(pos, 4);

	/* intermediate buffer size must be u32 multiple */
	size = ALIGN(count, 4);

	/* if start position is unaligned, read extra u32 */
	if (unlikely(pos != *ppos)) {
		skip = *ppos - pos;
		if (pos + size + 4 < dfse->size)
			size += 4;
	}

	buf = kzalloc(size, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	if (dfse->type == SOF_DFSENTRY_TYPE_IOMEM) {
#if IS_ENABLED(CONFIG_SND_SOC_SOF_DEBUG_ENABLE_DEBUGFS_CACHE)
		/*
		 * If the DSP is active: copy from IO.
		 * If the DSP is suspended:
		 *	- Copy from IO if the memory is always accessible.
		 *	- Otherwise, copy from cached buffer.
		 */
		if (pm_runtime_active(sdev->dev) ||
		    dfse->access_type == SOF_DEBUGFS_ACCESS_ALWAYS) {
			memcpy_fromio(buf, dfse->io_mem + pos, size);
		} else {
			dev_info(sdev->dev,
				 "Copying cached debugfs data\n");
			memcpy(buf, dfse->cache_buf + pos, size);
		}
#else
		/* if the DSP is in D3 */
		if (!pm_runtime_active(sdev->dev) &&
		    dfse->access_type == SOF_DEBUGFS_ACCESS_D0_ONLY) {
			dev_err(sdev->dev,
				"error: debugfs entry cannot be read in DSP D3\n");
			kfree(buf);
			return -EINVAL;
		}

		memcpy_fromio(buf, dfse->io_mem + pos, size);
#endif
	} else {
		memcpy(buf, ((u8 *)(dfse->buf) + pos), size);
	}

	/* copy to userspace */
	size_ret = copy_to_user(buffer, buf + skip, count);

	kfree(buf);

	/* update count & position if copy succeeded */
	if (size_ret)
		return -EFAULT;

	*ppos = pos + count;

	return count;
}