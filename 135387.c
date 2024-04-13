static int vt_resizex(struct vc_data *vc, struct vt_consize __user *cs)
{
	struct vt_consize v;
	int i;

	if (copy_from_user(&v, cs, sizeof(struct vt_consize)))
		return -EFAULT;

	/* FIXME: Should check the copies properly */
	if (!v.v_vlin)
		v.v_vlin = vc->vc_scan_lines;

	if (v.v_clin) {
		int rows = v.v_vlin / v.v_clin;
		if (v.v_rows != rows) {
			if (v.v_rows) /* Parameters don't add up */
				return -EINVAL;
			v.v_rows = rows;
		}
	}

	if (v.v_vcol && v.v_ccol) {
		int cols = v.v_vcol / v.v_ccol;
		if (v.v_cols != cols) {
			if (v.v_cols)
				return -EINVAL;
			v.v_cols = cols;
		}
	}

	if (v.v_clin > 32)
		return -EINVAL;

	for (i = 0; i < MAX_NR_CONSOLES; i++) {
		struct vc_data *vcp;

		if (!vc_cons[i].d)
			continue;
		console_lock();
		vcp = vc_cons[i].d;
		if (vcp) {
			int ret;
			int save_scan_lines = vcp->vc_scan_lines;
			int save_cell_height = vcp->vc_cell_height;

			if (v.v_vlin)
				vcp->vc_scan_lines = v.v_vlin;
			if (v.v_clin)
				vcp->vc_cell_height = v.v_clin;
			vcp->vc_resize_user = 1;
			ret = vc_resize(vcp, v.v_cols, v.v_rows);
			if (ret) {
				vcp->vc_scan_lines = save_scan_lines;
				vcp->vc_cell_height = save_cell_height;
				console_unlock();
				return ret;
			}
		}
		console_unlock();
	}

	return 0;
}