inflight_mem_alloc(const char *name, size_t size, int *fd)
{
	void *ptr;
	int mfd = -1;
	char fname[20] = "/tmp/memfd-XXXXXX";

	*fd = -1;
#ifdef MEMFD_SUPPORTED
	mfd = memfd_create(name, MFD_CLOEXEC);
#else
	RTE_SET_USED(name);
#endif
	if (mfd == -1) {
		mfd = mkstemp(fname);
		if (mfd == -1) {
			VHOST_LOG_CONFIG(ERR,
				"failed to get inflight buffer fd\n");
			return NULL;
		}

		unlink(fname);
	}

	if (ftruncate(mfd, size) == -1) {
		VHOST_LOG_CONFIG(ERR,
			"failed to alloc inflight buffer\n");
		close(mfd);
		return NULL;
	}

	ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, mfd, 0);
	if (ptr == MAP_FAILED) {
		VHOST_LOG_CONFIG(ERR,
			"failed to mmap inflight buffer\n");
		close(mfd);
		return NULL;
	}

	*fd = mfd;
	return ptr;
}