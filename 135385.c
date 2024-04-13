static void __vt_event_dequeue(struct vt_event_wait *vw)
{
	unsigned long flags;

	/* Dequeue it */
	spin_lock_irqsave(&vt_event_lock, flags);
	list_del(&vw->list);
	spin_unlock_irqrestore(&vt_event_lock, flags);
}