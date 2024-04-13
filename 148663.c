umount2(const char *path, int flags) {
	fprintf(stderr, _("umount: compiled without support for -f\n"));
	errno = ENOSYS;
	return -1;
}