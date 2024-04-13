int vbg_hgcm_call32(
	struct vbg_dev *gdev, u32 requestor, u32 client_id, u32 function,
	u32 timeout_ms, struct vmmdev_hgcm_function_parameter32 *parm32,
	u32 parm_count, int *vbox_status)
{
	struct vmmdev_hgcm_function_parameter *parm64 = NULL;
	u32 i, size;
	int ret = 0;

	/* KISS allocate a temporary request and convert the parameters. */
	size = parm_count * sizeof(struct vmmdev_hgcm_function_parameter);
	parm64 = kzalloc(size, GFP_KERNEL);
	if (!parm64)
		return -ENOMEM;

	for (i = 0; i < parm_count; i++) {
		switch (parm32[i].type) {
		case VMMDEV_HGCM_PARM_TYPE_32BIT:
			parm64[i].type = VMMDEV_HGCM_PARM_TYPE_32BIT;
			parm64[i].u.value32 = parm32[i].u.value32;
			break;

		case VMMDEV_HGCM_PARM_TYPE_64BIT:
			parm64[i].type = VMMDEV_HGCM_PARM_TYPE_64BIT;
			parm64[i].u.value64 = parm32[i].u.value64;
			break;

		case VMMDEV_HGCM_PARM_TYPE_LINADDR_OUT:
		case VMMDEV_HGCM_PARM_TYPE_LINADDR:
		case VMMDEV_HGCM_PARM_TYPE_LINADDR_IN:
			parm64[i].type = parm32[i].type;
			parm64[i].u.pointer.size = parm32[i].u.pointer.size;
			parm64[i].u.pointer.u.linear_addr =
			    parm32[i].u.pointer.u.linear_addr;
			break;

		default:
			ret = -EINVAL;
		}
		if (ret < 0)
			goto out_free;
	}

	ret = vbg_hgcm_call(gdev, requestor, client_id, function, timeout_ms,
			    parm64, parm_count, vbox_status);
	if (ret < 0)
		goto out_free;

	/* Copy back. */
	for (i = 0; i < parm_count; i++, parm32++, parm64++) {
		switch (parm64[i].type) {
		case VMMDEV_HGCM_PARM_TYPE_32BIT:
			parm32[i].u.value32 = parm64[i].u.value32;
			break;

		case VMMDEV_HGCM_PARM_TYPE_64BIT:
			parm32[i].u.value64 = parm64[i].u.value64;
			break;

		case VMMDEV_HGCM_PARM_TYPE_LINADDR_OUT:
		case VMMDEV_HGCM_PARM_TYPE_LINADDR:
		case VMMDEV_HGCM_PARM_TYPE_LINADDR_IN:
			parm32[i].u.pointer.size = parm64[i].u.pointer.size;
			break;

		default:
			WARN_ON(1);
			ret = -EINVAL;
		}
	}

out_free:
	kfree(parm64);
	return ret;
}