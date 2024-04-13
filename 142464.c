int vbg_status_code_to_errno(int rc)
{
	if (rc >= 0)
		return 0;

	rc = -rc;
	if (rc >= ARRAY_SIZE(vbg_status_code_to_errno_table) ||
	    vbg_status_code_to_errno_table[rc] == 0) {
		vbg_warn("%s: Unhandled err %d\n", __func__, -rc);
		return -EPROTO;
	}

	return vbg_status_code_to_errno_table[rc];
}