static void hgcm_call_init_call(
	struct vmmdev_hgcm_call *call, u32 client_id, u32 function,
	const struct vmmdev_hgcm_function_parameter *src_parm,
	u32 parm_count, void **bounce_bufs)
{
	struct vmmdev_hgcm_function_parameter *dst_parm =
		VMMDEV_HGCM_CALL_PARMS(call);
	u32 i, off_extra = (uintptr_t)(dst_parm + parm_count) - (uintptr_t)call;
	void *buf;

	call->header.flags = 0;
	call->header.result = VINF_SUCCESS;
	call->client_id = client_id;
	call->function = function;
	call->parm_count = parm_count;

	for (i = 0; i < parm_count; i++, src_parm++, dst_parm++) {
		switch (src_parm->type) {
		case VMMDEV_HGCM_PARM_TYPE_32BIT:
		case VMMDEV_HGCM_PARM_TYPE_64BIT:
			*dst_parm = *src_parm;
			break;

		case VMMDEV_HGCM_PARM_TYPE_LINADDR:
		case VMMDEV_HGCM_PARM_TYPE_LINADDR_IN:
		case VMMDEV_HGCM_PARM_TYPE_LINADDR_OUT:
			hgcm_call_init_linaddr(call, dst_parm, bounce_bufs[i],
					       src_parm->u.pointer.size,
					       src_parm->type, &off_extra);
			break;

		case VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL:
		case VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL_IN:
		case VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL_OUT:
			buf = (void *)src_parm->u.pointer.u.linear_addr;
			hgcm_call_init_linaddr(call, dst_parm, buf,
					       src_parm->u.pointer.size,
					       src_parm->type, &off_extra);
			break;

		default:
			WARN_ON(1);
			dst_parm->type = VMMDEV_HGCM_PARM_TYPE_INVALID;
		}
	}
}