static int aio_setup_rw(int rw, const struct iocb *iocb, struct iovec **iovec,
		bool vectored, bool compat, struct iov_iter *iter)
{
	void __user *buf = (void __user *)(uintptr_t)iocb->aio_buf;
	size_t len = iocb->aio_nbytes;

	if (!vectored) {
		ssize_t ret = import_single_range(rw, buf, len, *iovec, iter);
		*iovec = NULL;
		return ret;
	}
#ifdef CONFIG_COMPAT
	if (compat)
		return compat_import_iovec(rw, buf, len, UIO_FASTIOV, iovec,
				iter);
#endif
	return import_iovec(rw, buf, len, UIO_FASTIOV, iovec, iter);
}