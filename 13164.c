la_opendirat(int fd, const char *path) {
	const int flags = O_CLOEXEC
#if defined(O_BINARY)
	    | O_BINARY
#endif
#if defined(O_DIRECTORY)
	    | O_DIRECTORY
#endif
#if defined(O_PATH)
	    | O_PATH
#elif defined(O_SEARCH)
	    | O_SEARCH
#elif defined(__FreeBSD__) && defined(O_EXEC)
	    | O_EXEC
#else
	    | O_RDONLY
#endif
	    ;

#if !defined(HAVE_OPENAT)
	if (fd != AT_FDCWD) {
		errno = ENOTSUP;
		return (-1);
	} else
		return (open(path, flags));
#else
	return (openat(fd, path, flags));
#endif
}