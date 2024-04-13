aio_poll_queue_proc(struct file *file, struct wait_queue_head *head,
		struct poll_table_struct *p)
{
	struct aio_poll_table *pt = container_of(p, struct aio_poll_table, pt);

	/* multiple wait queues per file are not supported */
	if (unlikely(pt->iocb->poll.head)) {
		pt->error = -EINVAL;
		return;
	}

	pt->error = 0;
	pt->iocb->poll.head = head;
	add_wait_queue(head, &pt->iocb->poll.wait);
}