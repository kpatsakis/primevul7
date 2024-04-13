umount_one_bw (const char *file, struct mntentchn *mc0) {
	struct mntentchn *mc;
	int res = 1;

	mc = mc0;
	while (res && mc) {
		res = umount_one(mc->m.mnt_fsname, mc->m.mnt_dir,
				 mc->m.mnt_type, mc->m.mnt_opts, mc);
		mc = getmntdirbackward(file, mc);
	}
	mc = mc0;
	while (res && mc) {
		res = umount_one(mc->m.mnt_fsname, mc->m.mnt_dir,
				 mc->m.mnt_type, mc->m.mnt_opts, mc);
		mc = getmntdevbackward(file, mc);
	}
	return res;
}