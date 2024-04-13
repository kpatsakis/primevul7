static int hgcm_call_preprocess(
	const struct vmmdev_hgcm_function_parameter *src_parm,
	u32 parm_count, void ***bounce_bufs_ret, size_t *extra)
{
	void *buf, **bounce_bufs = NULL;
	u32 i, len;
	int ret;

	for (i = 0; i < parm_count; i++, src_parm++) {
		switch (src_parm->type) {
		case VMMDEV_HGCM_PARM_TYPE_32BIT:
		case VMMDEV_HGCM_PARM_TYPE_64BIT:
			break;

		case VMMDEV_HGCM_PARM_TYPE_LINADDR:
		case VMMDEV_HGCM_PARM_TYPE_LINADDR_IN:
		case VMMDEV_HGCM_PARM_TYPE_LINADDR_OUT:
			if (!bounce_bufs) {
				bounce_bufs = kcalloc(parm_count,
						      sizeof(void *),
						      GFP_KERNEL);
				if (!bounce_bufs)
					return -ENOMEM;

				*bounce_bufs_ret = bounce_bufs;
			}

			ret = hgcm_call_preprocess_linaddr(src_parm,
							   &bounce_bufs[i],
							   extra);
			if (ret)
				return ret;

			break;

		case VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL:
		case VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL_IN:
		case VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL_OUT:
			buf = (void *)src_parm->u.pointer.u.linear_addr;
			len = src_parm->u.pointer.size;
			if (WARN_ON(len > VBG_MAX_HGCM_KERNEL_PARM))
				return -E2BIG;

			hgcm_call_add_pagelist_size(buf, len, extra);
			break;

		default:
			return -EINVAL;
		}
	}

	return 0;
}