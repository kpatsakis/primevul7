SYSCALL_DEFINE3(io_cancel, aio_context_t, ctx_id, struct iocb __user *, iocb,
		struct io_event __user *, result)
{
	struct kioctx *ctx;
	struct aio_kiocb *kiocb;
	int ret = -EINVAL;
	u32 key;

	if (unlikely(get_user(key, &iocb->aio_key)))
		return -EFAULT;
	if (unlikely(key != KIOCB_KEY))
		return -EINVAL;

	ctx = lookup_ioctx(ctx_id);
	if (unlikely(!ctx))
		return -EINVAL;

	spin_lock_irq(&ctx->ctx_lock);
	kiocb = lookup_kiocb(ctx, iocb);
	if (kiocb) {
		ret = kiocb->ki_cancel(&kiocb->rw);
		list_del_init(&kiocb->ki_list);
	}
	spin_unlock_irq(&ctx->ctx_lock);

	if (!ret) {
		/*
		 * The result argument is no longer used - the io_event is
		 * always delivered via the ring buffer. -EINPROGRESS indicates
		 * cancellation is progress:
		 */
		ret = -EINPROGRESS;
	}

	percpu_ref_put(&ctx->users);

	return ret;
}