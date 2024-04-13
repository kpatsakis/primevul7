COMPAT_SYSCALL_DEFINE5(io_getevents, compat_aio_context_t, ctx_id,
		       compat_long_t, min_nr,
		       compat_long_t, nr,
		       struct io_event __user *, events,
		       struct old_timespec32 __user *, timeout)
{
	struct timespec64 t;
	int ret;

	if (timeout && get_old_timespec32(&t, timeout))
		return -EFAULT;

	ret = do_io_getevents(ctx_id, min_nr, nr, events, timeout ? &t : NULL);
	if (!ret && signal_pending(current))
		ret = -EINTR;
	return ret;
}