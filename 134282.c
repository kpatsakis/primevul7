static int aio_poll_wake(struct wait_queue_entry *wait, unsigned mode, int sync,
		void *key)
{
	struct poll_iocb *req = container_of(wait, struct poll_iocb, wait);
	struct aio_kiocb *iocb = container_of(req, struct aio_kiocb, poll);
	__poll_t mask = key_to_poll(key);

	req->woken = true;

	/* for instances that support it check for an event match first: */
	if (mask) {
		if (!(mask & req->events))
			return 0;

		/* try to complete the iocb inline if we can: */
		if (spin_trylock(&iocb->ki_ctx->ctx_lock)) {
			list_del(&iocb->ki_list);
			spin_unlock(&iocb->ki_ctx->ctx_lock);

			list_del_init(&req->wait.entry);
			aio_poll_complete(iocb, mask);
			return 1;
		}
	}

	list_del_init(&req->wait.entry);
	schedule_work(&req->work);
	return 1;
}