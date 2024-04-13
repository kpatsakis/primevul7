SYSCALL_DEFINE6(io_pgetevents_time32,
		aio_context_t, ctx_id,
		long, min_nr,
		long, nr,
		struct io_event __user *, events,
		struct old_timespec32 __user *, timeout,
		const struct __aio_sigset __user *, usig)
{
	struct __aio_sigset	ksig = { NULL, };
	sigset_t		ksigmask, sigsaved;
	struct timespec64	ts;
	int ret;

	if (timeout && unlikely(get_old_timespec32(&ts, timeout)))
		return -EFAULT;

	if (usig && copy_from_user(&ksig, usig, sizeof(ksig)))
		return -EFAULT;


	ret = set_user_sigmask(ksig.sigmask, &ksigmask, &sigsaved, ksig.sigsetsize);
	if (ret)
		return ret;

	ret = do_io_getevents(ctx_id, min_nr, nr, events, timeout ? &ts : NULL);
	restore_user_sigmask(ksig.sigmask, &sigsaved);
	if (signal_pending(current) && !ret)
		ret = -ERESTARTNOHAND;

	return ret;
}