SYSCALL_DEFINE5(io_getevents, aio_context_t, ctx_id,
		long, min_nr,
		long, nr,
		struct io_event __user *, events,
		struct __kernel_timespec __user *, timeout)
{
	struct timespec64	ts;
	int			ret;

	if (timeout && unlikely(get_timespec64(&ts, timeout)))
		return -EFAULT;

	ret = do_io_getevents(ctx_id, min_nr, nr, events, timeout ? &ts : NULL);
	if (!ret && signal_pending(current))
		ret = -EINTR;
	return ret;
}