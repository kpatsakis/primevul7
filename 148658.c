check_special_umountprog(const char *spec, const char *node,
			 const char *type, int *status) {
	char umountprog[120];
	struct stat statbuf;
	int res;

	if (!external_allowed)
		return 0;

	if (type == NULL || strcmp(type, "none") == 0)
		return 0;

	if (strlen(type) < 100) {
		int type_opt = 0;

		sprintf(umountprog, "/sbin/umount.%s", type);
		res = stat(umountprog, &statbuf);
		if (res == -1 && errno == ENOENT && strchr(type, '.')) {
			/* If type ends with ".subtype" try without it */
			*strrchr(umountprog, '.') = '\0';
			type_opt = 1;
			res = stat(umountprog, &statbuf);
		}
		if (res == 0) {
			res = fork();
			if (res == 0) {
				char *umountargs[10];
				int i = 0;

				if(setgid(getgid()) < 0)
					die(EX_FAIL, _("umount: cannot set group id: %s"), strerror(errno));

				if(setuid(getuid()) < 0)
					die(EX_FAIL, _("umount: cannot set user id: %s"), strerror(errno));

				umountargs[i++] = umountprog;
				umountargs[i++] = xstrdup(node);
				if (nomtab)
					umountargs[i++] = "-n";
				if (lazy)
					umountargs[i++] = "-l";
				if (force)
					umountargs[i++] = "-f";
				if (verbose)
					umountargs[i++] = "-v";
				if (remount)
					umountargs[i++] = "-r";
				if (type_opt) {
					umountargs[i++] = "-t";
					umountargs[i++] = (char *) type;
				}
				umountargs[i] = NULL;
				execv(umountprog, umountargs);
				exit(1);	/* exec failed */
			} else if (res != -1) {
				int st;
				wait(&st);
				*status = (WIFEXITED(st) ? WEXITSTATUS(st)
					   : EX_SYSERR);
				return 1;
			} else {
				int errsv = errno;
				error(_("umount: cannot fork: %s"),
				      strerror(errsv));
			}
		}
	}
	return 0;
}