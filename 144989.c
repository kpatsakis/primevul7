debug(pam_handle_t *pamh, const struct if_dqblk *p,
      const char *device, const char *dbgprefix) {
  pam_syslog(pamh, LOG_DEBUG, "%s device=%s bsoftlimit=%llu bhardlimit=%llu "
                              "isoftlimit=%llu ihardlimit=%llu btime=%llu itime=%llu",
	     dbgprefix, device,
	     (unsigned long long) p->dqb_bsoftlimit,
	     (unsigned long long) p->dqb_bhardlimit,
	     (unsigned long long) p->dqb_isoftlimit,
	     (unsigned long long) p->dqb_ihardlimit,
	     (unsigned long long) p->dqb_btime,
	     (unsigned long long) p->dqb_itime);
}