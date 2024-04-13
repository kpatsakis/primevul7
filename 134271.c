static void aio_poll_complete_work(struct work_struct *work)
{
	struct poll_iocb *req = container_of(work, struct poll_iocb, work);
	struct aio_kiocb *iocb = container_of(req, struct aio_kiocb, poll);
	struct poll_table_struct pt = { ._key = req->events };
	struct kioctx *ctx = iocb->ki_ctx;
	__poll_t mask = 0;

	if (!READ_ONCE(req->cancelled))
		mask = vfs_poll(req->file, &pt) & req->events;

	/*
	 * Note that ->ki_cancel callers also delete iocb from active_reqs after
	 * calling ->ki_cancel.  We need the ctx_lock roundtrip here to
	 * synchronize with them.  In the cancellation case the list_del_init
	 * itself is not actually needed, but harmless so we keep it in to
	 * avoid further branches in the fast path.
	 */
	spin_lock_irq(&ctx->ctx_lock);
	if (!mask && !READ_ONCE(req->cancelled)) {
		add_wait_queue(req->head, &req->wait);
		spin_unlock_irq(&ctx->ctx_lock);
		return;
	}
	list_del_init(&iocb->ki_list);
	spin_unlock_irq(&ctx->ctx_lock);

	aio_poll_complete(iocb, mask);
}