static int vt_setactivate(struct vt_setactivate __user *sa)
{
	struct vt_setactivate vsa;
	struct vc_data *nvc;
	int ret;

	if (copy_from_user(&vsa, sa, sizeof(vsa)))
		return -EFAULT;
	if (vsa.console == 0 || vsa.console > MAX_NR_CONSOLES)
		return -ENXIO;

	vsa.console = array_index_nospec(vsa.console, MAX_NR_CONSOLES + 1);
	vsa.console--;
	console_lock();
	ret = vc_allocate(vsa.console);
	if (ret) {
		console_unlock();
		return ret;
	}

	/*
	 * This is safe providing we don't drop the console sem between
	 * vc_allocate and finishing referencing nvc.
	 */
	nvc = vc_cons[vsa.console].d;
	nvc->vt_mode = vsa.mode;
	nvc->vt_mode.frsig = 0;
	put_pid(nvc->vt_pid);
	nvc->vt_pid = get_pid(task_pid(current));
	console_unlock();

	/* Commence switch and lock */
	/* Review set_console locks */
	set_console(vsa.console);

	return 0;
}