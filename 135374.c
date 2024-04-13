static int vt_reldisp(struct vc_data *vc, unsigned int swtch)
{
	int newvt, ret;

	if (vc->vt_mode.mode != VT_PROCESS)
		return -EINVAL;

	/* Switched-to response */
	if (vc->vt_newvt < 0) {
		 /* If it's just an ACK, ignore it */
		return swtch == VT_ACKACQ ? 0 : -EINVAL;
	}

	/* Switching-from response */
	if (swtch == 0) {
		/* Switch disallowed, so forget we were trying to do it. */
		vc->vt_newvt = -1;
		return 0;
	}

	/* The current vt has been released, so complete the switch. */
	newvt = vc->vt_newvt;
	vc->vt_newvt = -1;
	ret = vc_allocate(newvt);
	if (ret)
		return ret;

	/*
	 * When we actually do the console switch, make sure we are atomic with
	 * respect to other console switches..
	 */
	complete_change_console(vc_cons[newvt].d);

	return 0;
}