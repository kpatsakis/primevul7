void vt_event_post(unsigned int event, unsigned int old, unsigned int new)
{
	struct list_head *pos, *head;
	unsigned long flags;
	int wake = 0;

	spin_lock_irqsave(&vt_event_lock, flags);
	head = &vt_events;

	list_for_each(pos, head) {
		struct vt_event_wait *ve = list_entry(pos,
						struct vt_event_wait, list);
		if (!(ve->event.event & event))
			continue;
		ve->event.event = event;
		/* kernel view is consoles 0..n-1, user space view is
		   console 1..n with 0 meaning current, so we must bias */
		ve->event.oldev = old + 1;
		ve->event.newev = new + 1;
		wake = 1;
		ve->done = 1;
	}
	spin_unlock_irqrestore(&vt_event_lock, flags);
	if (wake)
		wake_up_interruptible(&vt_event_waitqueue);
}