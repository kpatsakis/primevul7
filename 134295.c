static int io_submit_one(struct kioctx *ctx, struct iocb __user *user_iocb,
			 bool compat)
{
	struct iocb iocb;

	if (unlikely(copy_from_user(&iocb, user_iocb, sizeof(iocb))))
		return -EFAULT;

	return __io_submit_one(ctx, &iocb, user_iocb, compat);
}