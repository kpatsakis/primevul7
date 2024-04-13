static long aspeed_lpc_ctrl_ioctl(struct file *file, unsigned int cmd,
		unsigned long param)
{
	struct aspeed_lpc_ctrl *lpc_ctrl = file_aspeed_lpc_ctrl(file);
	struct device *dev = file->private_data;
	void __user *p = (void __user *)param;
	struct aspeed_lpc_ctrl_mapping map;
	u32 addr;
	u32 size;
	long rc;

	if (copy_from_user(&map, p, sizeof(map)))
		return -EFAULT;

	if (map.flags != 0)
		return -EINVAL;

	switch (cmd) {
	case ASPEED_LPC_CTRL_IOCTL_GET_SIZE:
		/* The flash windows don't report their size */
		if (map.window_type != ASPEED_LPC_CTRL_WINDOW_MEMORY)
			return -EINVAL;

		/* Support more than one window id in the future */
		if (map.window_id != 0)
			return -EINVAL;

		/* If memory-region is not described in device tree */
		if (!lpc_ctrl->mem_size) {
			dev_dbg(dev, "Didn't find reserved memory\n");
			return -ENXIO;
		}

		map.size = lpc_ctrl->mem_size;

		return copy_to_user(p, &map, sizeof(map)) ? -EFAULT : 0;
	case ASPEED_LPC_CTRL_IOCTL_MAP:

		/*
		 * The top half of HICR7 is the MSB of the BMC address of the
		 * mapping.
		 * The bottom half of HICR7 is the MSB of the HOST LPC
		 * firmware space address of the mapping.
		 *
		 * The 1 bits in the top of half of HICR8 represent the bits
		 * (in the requested address) that should be ignored and
		 * replaced with those from the top half of HICR7.
		 * The 1 bits in the bottom half of HICR8 represent the bits
		 * (in the requested address) that should be kept and pass
		 * into the BMC address space.
		 */

		/*
		 * It doesn't make sense to talk about a size or offset with
		 * low 16 bits set. Both HICR7 and HICR8 talk about the top 16
		 * bits of addresses and sizes.
		 */

		if ((map.size & 0x0000ffff) || (map.offset & 0x0000ffff))
			return -EINVAL;

		/*
		 * Because of the way the masks work in HICR8 offset has to
		 * be a multiple of size.
		 */
		if (map.offset & (map.size - 1))
			return -EINVAL;

		if (map.window_type == ASPEED_LPC_CTRL_WINDOW_FLASH) {
			if (!lpc_ctrl->pnor_size) {
				dev_dbg(dev, "Didn't find host pnor flash\n");
				return -ENXIO;
			}
			addr = lpc_ctrl->pnor_base;
			size = lpc_ctrl->pnor_size;
		} else if (map.window_type == ASPEED_LPC_CTRL_WINDOW_MEMORY) {
			/* If memory-region is not described in device tree */
			if (!lpc_ctrl->mem_size) {
				dev_dbg(dev, "Didn't find reserved memory\n");
				return -ENXIO;
			}
			addr = lpc_ctrl->mem_base;
			size = lpc_ctrl->mem_size;
		} else {
			return -EINVAL;
		}

		/* Check overflow first! */
		if (map.offset + map.size < map.offset ||
			map.offset + map.size > size)
			return -EINVAL;

		if (map.size == 0 || map.size > size)
			return -EINVAL;

		addr += map.offset;

		/*
		 * addr (host lpc address) is safe regardless of values. This
		 * simply changes the address the host has to request on its
		 * side of the LPC bus. This cannot impact the hosts own
		 * memory space by surprise as LPC specific accessors are
		 * required. The only strange thing that could be done is
		 * setting the lower 16 bits but the shift takes care of that.
		 */

		rc = regmap_write(lpc_ctrl->regmap, HICR7,
				(addr | (map.addr >> 16)));
		if (rc)
			return rc;

		rc = regmap_write(lpc_ctrl->regmap, HICR8,
				(~(map.size - 1)) | ((map.size >> 16) - 1));
		if (rc)
			return rc;

		/*
		 * Switch to FWH2AHB mode, AST2600 only.
		 *
		 * The other bits in this register are interrupt status bits
		 * that are cleared by writing 1. As we don't want to clear
		 * them, set only the bit of interest.
		 */
		if (lpc_ctrl->fwh2ahb)
			regmap_write(lpc_ctrl->regmap, HICR6, SW_FWH2AHB);

		/*
		 * Enable LPC FHW cycles. This is required for the host to
		 * access the regions specified.
		 */
		return regmap_update_bits(lpc_ctrl->regmap, HICR5,
				HICR5_ENFWH | HICR5_ENL2H,
				HICR5_ENFWH | HICR5_ENL2H);
	}

	return -EINVAL;
}