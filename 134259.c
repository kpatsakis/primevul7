static int aio_poll_cancel(struct kiocb *iocb)
{
	struct aio_kiocb *aiocb = container_of(iocb, struct aio_kiocb, rw);
	struct poll_iocb *req = &aiocb->poll;

	spin_lock(&req->head->lock);
	WRITE_ONCE(req->cancelled, true);
	if (!list_empty(&req->wait.entry)) {
		list_del_init(&req->wait.entry);
		schedule_work(&aiocb->poll.work);
	}
	spin_unlock(&req->head->lock);

	return 0;
}