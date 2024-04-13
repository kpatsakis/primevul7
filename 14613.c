monotime_ts(struct timespec *ts)
{
	struct timeval tv;
#if defined(HAVE_CLOCK_GETTIME) && (defined(CLOCK_BOOTTIME) || \
    defined(CLOCK_MONOTONIC) || defined(CLOCK_REALTIME))
	static int gettime_failed = 0;

	if (!gettime_failed) {
# ifdef CLOCK_BOOTTIME
		if (clock_gettime(CLOCK_BOOTTIME, ts) == 0)
			return;
# endif /* CLOCK_BOOTTIME */
# ifdef CLOCK_MONOTONIC
		if (clock_gettime(CLOCK_MONOTONIC, ts) == 0)
			return;
# endif /* CLOCK_MONOTONIC */
# ifdef CLOCK_REALTIME
		/* Not monotonic, but we're almost out of options here. */
		if (clock_gettime(CLOCK_REALTIME, ts) == 0)
			return;
# endif /* CLOCK_REALTIME */
		debug3("clock_gettime: %s", strerror(errno));
		gettime_failed = 1;
	}
#endif /* HAVE_CLOCK_GETTIME && (BOOTTIME || MONOTONIC || REALTIME) */
	gettimeofday(&tv, NULL);
	ts->tv_sec = tv.tv_sec;
	ts->tv_nsec = (long)tv.tv_usec * 1000;
}