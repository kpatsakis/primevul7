static void __vt_event_wait(struct vt_event_wait *vw)
{
	/* Wait for it to pass */
	wait_event_interruptible(vt_event_waitqueue, vw->done);
}