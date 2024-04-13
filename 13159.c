set_ownership(struct archive_write_disk *a)
{
#if !defined(__CYGWIN__) && !defined(__linux__)
/*
 * On Linux, a process may have the CAP_CHOWN capability.
 * On Windows there is no 'root' user with uid 0.
 * Elsewhere we can skip calling chown if we are not root and the desired
 * user id does not match the current user.
 */
	if (a->user_uid != 0 && a->user_uid != a->uid) {
		archive_set_error(&a->archive, errno,
		    "Can't set UID=%jd", (intmax_t)a->uid);
		return (ARCHIVE_WARN);
	}
#endif

#ifdef HAVE_FCHOWN
	/* If we have an fd, we can avoid a race. */
	if (a->fd >= 0 && fchown(a->fd, a->uid, a->gid) == 0) {
		/* We've set owner and know uid/gid are correct. */
		a->todo &= ~(TODO_OWNER | TODO_SGID_CHECK | TODO_SUID_CHECK);
		return (ARCHIVE_OK);
	}
#endif

	/* We prefer lchown() but will use chown() if that's all we have. */
	/* Of course, if we have neither, this will always fail. */
#ifdef HAVE_LCHOWN
	if (lchown(a->name, a->uid, a->gid) == 0) {
		/* We've set owner and know uid/gid are correct. */
		a->todo &= ~(TODO_OWNER | TODO_SGID_CHECK | TODO_SUID_CHECK);
		return (ARCHIVE_OK);
	}
#elif HAVE_CHOWN
	if (!S_ISLNK(a->mode) && chown(a->name, a->uid, a->gid) == 0) {
		/* We've set owner and know uid/gid are correct. */
		a->todo &= ~(TODO_OWNER | TODO_SGID_CHECK | TODO_SUID_CHECK);
		return (ARCHIVE_OK);
	}
#endif

	archive_set_error(&a->archive, errno,
	    "Can't set user=%jd/group=%jd for %s",
	    (intmax_t)a->uid, (intmax_t)a->gid, a->name);
	return (ARCHIVE_WARN);
}