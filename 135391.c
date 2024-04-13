static int vt_event_wait_ioctl(struct vt_event __user *event)
{
	struct vt_event_wait vw;

	if (copy_from_user(&vw.event, event, sizeof(struct vt_event)))
		return -EFAULT;
	/* Highest supported event for now */
	if (vw.event.event & ~VT_MAX_EVENT)
		return -EINVAL;

	vt_event_wait(&vw);
	/* If it occurred report it */
	if (vw.done) {
		if (copy_to_user(event, &vw.event, sizeof(struct vt_event)))
			return -EFAULT;
		return 0;
	}
	return -EINTR;
}