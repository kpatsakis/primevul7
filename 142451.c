static u32 hgcm_call_linear_addr_type_to_pagelist_flags(
	enum vmmdev_hgcm_function_parameter_type type)
{
	switch (type) {
	default:
		WARN_ON(1);
		/* Fall through */
	case VMMDEV_HGCM_PARM_TYPE_LINADDR:
	case VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL:
		return VMMDEV_HGCM_F_PARM_DIRECTION_BOTH;

	case VMMDEV_HGCM_PARM_TYPE_LINADDR_IN:
	case VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL_IN:
		return VMMDEV_HGCM_F_PARM_DIRECTION_TO_HOST;

	case VMMDEV_HGCM_PARM_TYPE_LINADDR_OUT:
	case VMMDEV_HGCM_PARM_TYPE_LINADDR_KERNEL_OUT:
		return VMMDEV_HGCM_F_PARM_DIRECTION_FROM_HOST;
	}
}