str_to_dqb_num(pam_handle_t *pamh, const char *str, const char *param) {
  char *ep = NULL;

  errno = 0;
  long long temp = strtoll(str, &ep, 10);
  if (temp < 0 || str == ep || *ep != '\0' || errno !=0) {
    pam_syslog(pamh, LOG_ERR, "Parameter \"%s=%s\" invalid, setting to 0", param, str);
    return 0;
  }
  else {
    return temp;
  }
}