static int hgcm_call_copy_back_result(
	const struct vmmdev_hgcm_call *call,
	struct vmmdev_hgcm_function_parameter *dst_parm,
	u32 parm_count, void **bounce_bufs)
{
	const struct vmmdev_hgcm_function_parameter *src_parm =
		VMMDEV_HGCM_CALL_PARMS(call);
	void __user *p;
	int ret;
	u32 i;

	/* Copy back parameters. */
	for (i = 0; i < parm_count; i++, src_parm++, dst_parm++) {
		switch (dst_parm->type) {
		case VMMDEV_HGCM_PARM_TYPE_32BIT:
		case VMMDEV_HGCM_PARM_TYPE_64BIT:
			*dst_parm = *src_parm;
			break;

		case VMMDEV_HGCM_PARM_TYPE_PAGELIST:
			dst_parm->u.page_list.size = src_parm->u.page_list.size;
			break;

		case VMMDEV_HGCM_PARM_TYPE_LINADDR_IN:
		case VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL:
		case VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL_IN:
		case VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL_OUT:
			dst_parm->u.pointer.size = src_parm->u.pointer.size;
			break;

		case VMMDEV_HGCM_PARM_TYPE_LINADDR:
		case VMMDEV_HGCM_PARM_TYPE_LINADDR_OUT:
			dst_parm->u.pointer.size = src_parm->u.pointer.size;

			p = (void __user *)dst_parm->u.pointer.u.linear_addr;
			ret = copy_to_user(p, bounce_bufs[i],
					   min(src_parm->u.pointer.size,
					       dst_parm->u.pointer.size));
			if (ret)
				return -EFAULT;
			break;

		default:
			WARN_ON(1);
			return -EINVAL;
		}
	}

	return 0;
}