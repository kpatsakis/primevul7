is_valid_loop(struct mntentchn *mc, struct mntentchn *fs)
{
	uintmax_t offset = 0;
	char *p;

	/* check if it begins with /dev/loop */
	if (strncmp(mc->m.mnt_fsname, _PATH_DEV_LOOP,
				sizeof(_PATH_DEV_LOOP) - 1))
		return 0;

	/* check for loop option in fstab */
	if (!contains(fs->m.mnt_opts, "loop"))
		return 0;

	/* check for offset option in fstab */
	p = get_option_value(fs->m.mnt_opts, "offset=");
	if (p && strtosize(p, &offset)) {
		if (verbose > 1)
			printf(_("failed to parse 'offset=%s' options\n"), p);
		return 0;
	}

	/* check association */
	if (loopfile_used_with((char *) mc->m.mnt_fsname,
				fs->m.mnt_fsname, offset) == 1) {
		if (verbose > 1)
			printf(_("device %s is associated with %s\n"),
			       mc->m.mnt_fsname, fs->m.mnt_fsname);
		return 1;
	}

	if (verbose > 1)
		printf(_("device %s is not associated with %s\n"),
		       mc->m.mnt_fsname, fs->m.mnt_fsname);
	return 0;
}