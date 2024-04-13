SYSCALL_DEFINE3(io_submit, aio_context_t, ctx_id, long, nr,
		struct iocb __user * __user *, iocbpp)
{
	struct kioctx *ctx;
	long ret = 0;
	int i = 0;
	struct blk_plug plug;

	if (unlikely(nr < 0))
		return -EINVAL;

	ctx = lookup_ioctx(ctx_id);
	if (unlikely(!ctx)) {
		pr_debug("EINVAL: invalid context id\n");
		return -EINVAL;
	}

	if (nr > ctx->nr_events)
		nr = ctx->nr_events;

	if (nr > AIO_PLUG_THRESHOLD)
		blk_start_plug(&plug);
	for (i = 0; i < nr; i++) {
		struct iocb __user *user_iocb;

		if (unlikely(get_user(user_iocb, iocbpp + i))) {
			ret = -EFAULT;
			break;
		}

		ret = io_submit_one(ctx, user_iocb, false);
		if (ret)
			break;
	}
	if (nr > AIO_PLUG_THRESHOLD)
		blk_finish_plug(&plug);

	percpu_ref_put(&ctx->users);
	return i ? i : ret;
}