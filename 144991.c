parse_dqblk(pam_handle_t *pamh, int argc, const char **argv, struct if_dqblk *p) {
  bool bhard = false, bsoft = false, ihard = false, isoft = false;

  /* step through arguments */
  for (; argc-- > 0; ++argv) {
    const char *str;
    if ((str = pam_str_skip_prefix(*argv, "bhardlimit=")) != NULL) {
      p->dqb_bhardlimit = str_to_dqb_num(pamh, str, "bhardlimit");
      p->dqb_valid |= QIF_BLIMITS;
      bhard = true;
    } else if ((str = pam_str_skip_prefix(*argv, "bsoftlimit=")) != NULL) {
      p->dqb_bsoftlimit = str_to_dqb_num(pamh, str, "bsoftlimit");
      p->dqb_valid |= QIF_BLIMITS;
      bsoft = true;
    } else if ((str = pam_str_skip_prefix(*argv, "ihardlimit=")) != NULL) {
      p->dqb_ihardlimit = str_to_dqb_num(pamh, str, "ihardlimit");
      p->dqb_valid |= QIF_ILIMITS;
      ihard = true;
    } else if ((str = pam_str_skip_prefix(*argv, "isoftlimit=")) != NULL) {
      p->dqb_isoftlimit = str_to_dqb_num(pamh, str, "isoftlimit");
      p->dqb_valid |= QIF_ILIMITS;
      isoft = true;
    } else if ((str = pam_str_skip_prefix(*argv, "btime=")) != NULL) {
      p->dqb_btime = str_to_dqb_num(pamh, str, "btime");
      p->dqb_valid |= QIF_BTIME;
    } else if ((str = pam_str_skip_prefix(*argv, "itime=")) != NULL) {
      p->dqb_itime = str_to_dqb_num(pamh, str, "itime");
      p->dqb_valid |= QIF_ITIME;
    }
  }

  /* return FALSE if a softlimit or hardlimit has been set
   * independently of its counterpart.
   */
  return !(bhard ^ bsoft) && !(ihard ^ isoft);
}