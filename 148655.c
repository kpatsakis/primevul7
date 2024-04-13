umount_all (char *types, char *test_opts) {
     struct mntentchn *mc, *hd;
     int errors = 0;

     hd = mtab_head();
     if (!hd->prev)
	  die (2, _("umount: cannot find list of filesystems to unmount"));
     for (mc = hd->prev; mc != hd; mc = mc->prev) {
	  if (matching_type (mc->m.mnt_type, types)
	      && matching_opts (mc->m.mnt_opts, test_opts)) {
	       errors |= umount_one (mc->m.mnt_fsname, mc->m.mnt_dir,
				     mc->m.mnt_type, mc->m.mnt_opts, mc);
	  }
     }

     return errors;
}