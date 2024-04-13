COMPAT_SYSCALL_DEFINE6(io_pgetevents_time64,
		compat_aio_context_t, ctx_id,
		compat_long_t, min_nr,
		compat_long_t, nr,
		struct io_event __user *, events,
		struct __kernel_timespec __user *, timeout,
		const struct __compat_aio_sigset __user *, usig)
{
	struct __compat_aio_sigset ksig = { NULL, };
	sigset_t ksigmask, sigsaved;
	struct timespec64 t;
	int ret;

	if (timeout && get_timespec64(&t, timeout))
		return -EFAULT;

	if (usig && copy_from_user(&ksig, usig, sizeof(ksig)))
		return -EFAULT;

	ret = set_compat_user_sigmask(ksig.sigmask, &ksigmask, &sigsaved, ksig.sigsetsize);
	if (ret)
		return ret;

	ret = do_io_getevents(ctx_id, min_nr, nr, events, timeout ? &t : NULL);
	restore_user_sigmask(ksig.sigmask, &sigsaved);
	if (signal_pending(current) && !ret)
		ret = -ERESTARTNOHAND;

	return ret;
}