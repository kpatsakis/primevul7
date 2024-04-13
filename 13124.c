older(struct stat *st, struct archive_entry *entry)
{
	/* First, test the seconds and return if we have a definite answer. */
	/* Definitely older. */
	if (to_int64_time(st->st_mtime) < to_int64_time(archive_entry_mtime(entry)))
		return (1);
	/* Definitely younger. */
	if (to_int64_time(st->st_mtime) > to_int64_time(archive_entry_mtime(entry)))
		return (0);
	/* If this platform supports fractional seconds, try those. */
#if HAVE_STRUCT_STAT_ST_MTIMESPEC_TV_NSEC
	/* Definitely older. */
	if (st->st_mtimespec.tv_nsec < archive_entry_mtime_nsec(entry))
		return (1);
#elif HAVE_STRUCT_STAT_ST_MTIM_TV_NSEC
	/* Definitely older. */
	if (st->st_mtim.tv_nsec < archive_entry_mtime_nsec(entry))
		return (1);
#elif HAVE_STRUCT_STAT_ST_MTIME_N
	/* older. */
	if (st->st_mtime_n < archive_entry_mtime_nsec(entry))
		return (1);
#elif HAVE_STRUCT_STAT_ST_UMTIME
	/* older. */
	if (st->st_umtime * 1000 < archive_entry_mtime_nsec(entry))
		return (1);
#elif HAVE_STRUCT_STAT_ST_MTIME_USEC
	/* older. */
	if (st->st_mtime_usec * 1000 < archive_entry_mtime_nsec(entry))
		return (1);
#else
	/* This system doesn't have high-res timestamps. */
#endif
	/* Same age or newer, so not older. */
	return (0);
}