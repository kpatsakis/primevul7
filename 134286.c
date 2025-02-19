static void free_ioctx_users(struct percpu_ref *ref)
{
	struct kioctx *ctx = container_of(ref, struct kioctx, users);
	struct aio_kiocb *req;

	spin_lock_irq(&ctx->ctx_lock);

	while (!list_empty(&ctx->active_reqs)) {
		req = list_first_entry(&ctx->active_reqs,
				       struct aio_kiocb, ki_list);
		req->ki_cancel(&req->rw);
		list_del_init(&req->ki_list);
	}

	spin_unlock_irq(&ctx->ctx_lock);

	percpu_ref_kill(&ctx->reqs);
	percpu_ref_put(&ctx->reqs);
}