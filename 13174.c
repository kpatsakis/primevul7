set_time(int fd, int mode, const char *name,
    time_t atime, long atime_nsec,
    time_t mtime, long mtime_nsec)
{
	/* Select the best implementation for this platform. */
#if defined(HAVE_UTIMENSAT) && defined(HAVE_FUTIMENS)
	/*
	 * utimensat() and futimens() are defined in
	 * POSIX.1-2008. They support ns resolution and setting times
	 * on fds and symlinks.
	 */
	struct timespec ts[2];
	(void)mode; /* UNUSED */
	ts[0].tv_sec = atime;
	ts[0].tv_nsec = atime_nsec;
	ts[1].tv_sec = mtime;
	ts[1].tv_nsec = mtime_nsec;
	if (fd >= 0)
		return futimens(fd, ts);
	return utimensat(AT_FDCWD, name, ts, AT_SYMLINK_NOFOLLOW);

#elif HAVE_UTIMES
	/*
	 * The utimes()-family functions support µs-resolution and
	 * setting times fds and symlinks.  utimes() is documented as
	 * LEGACY by POSIX, futimes() and lutimes() are not described
	 * in POSIX.
	 */
	struct timeval times[2];

	times[0].tv_sec = atime;
	times[0].tv_usec = atime_nsec / 1000;
	times[1].tv_sec = mtime;
	times[1].tv_usec = mtime_nsec / 1000;

#ifdef HAVE_FUTIMES
	if (fd >= 0)
		return (futimes(fd, times));
#else
	(void)fd; /* UNUSED */
#endif
#ifdef HAVE_LUTIMES
	(void)mode; /* UNUSED */
	return (lutimes(name, times));
#else
	if (S_ISLNK(mode))
		return (0);
	return (utimes(name, times));
#endif

#elif defined(HAVE_UTIME)
	/*
	 * utime() is POSIX-standard but only supports 1s resolution and
	 * does not support fds or symlinks.
	 */
	struct utimbuf times;
	(void)fd; /* UNUSED */
	(void)name; /* UNUSED */
	(void)atime_nsec; /* UNUSED */
	(void)mtime_nsec; /* UNUSED */
	times.actime = atime;
	times.modtime = mtime;
	if (S_ISLNK(mode))
		return (ARCHIVE_OK);
	return (utime(name, &times));

#else
	/*
	 * We don't know how to set the time on this platform.
	 */
	(void)fd; /* UNUSED */
	(void)mode; /* UNUSED */
	(void)name; /* UNUSED */
	(void)atime_nsec; /* UNUSED */
	(void)mtime_nsec; /* UNUSED */
	return (ARCHIVE_WARN);
#endif
}