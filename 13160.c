set_time_tru64(int fd, int mode, const char *name,
    time_t atime, long atime_nsec,
    time_t mtime, long mtime_nsec,
    time_t ctime, long ctime_nsec)
{
	struct attr_timbuf tstamp;
	tstamp.atime.tv_sec = atime;
	tstamp.mtime.tv_sec = mtime;
	tstamp.ctime.tv_sec = ctime;
#if defined (__hpux) && defined (__ia64)
	tstamp.atime.tv_nsec = atime_nsec;
	tstamp.mtime.tv_nsec = mtime_nsec;
	tstamp.ctime.tv_nsec = ctime_nsec;
#else
	tstamp.atime.tv_usec = atime_nsec / 1000;
	tstamp.mtime.tv_usec = mtime_nsec / 1000;
	tstamp.ctime.tv_usec = ctime_nsec / 1000;
#endif
	return (fcntl(fd,F_SETTIMES,&tstamp));
}