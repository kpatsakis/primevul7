void vc_SAK(struct work_struct *work)
{
	struct vc *vc_con =
		container_of(work, struct vc, SAK_work);
	struct vc_data *vc;
	struct tty_struct *tty;

	console_lock();
	vc = vc_con->d;
	if (vc) {
		/* FIXME: review tty ref counting */
		tty = vc->port.tty;
		/*
		 * SAK should also work in all raw modes and reset
		 * them properly.
		 */
		if (tty)
			__do_SAK(tty);
		reset_vc(vc);
	}
	console_unlock();
}