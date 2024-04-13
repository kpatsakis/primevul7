static bool tld_check_name(struct SessionHandle *data,
                           const char *ace_hostname)
{
  size_t err_pos;
  char *uc_name = NULL;
  int rc;
#ifndef CURL_DISABLE_VERBOSE_STRINGS
  const char *tld_errmsg = "<no msg>";
#else
  (void)data;
#endif

  /* Convert (and downcase) ACE-name back into locale's character set */
  rc = idna_to_unicode_lzlz(ace_hostname, &uc_name, 0);
  if(rc != IDNA_SUCCESS)
    return FALSE;

  rc = tld_check_lz(uc_name, &err_pos, NULL);
#ifndef CURL_DISABLE_VERBOSE_STRINGS
#ifdef HAVE_TLD_STRERROR
  if(rc != TLD_SUCCESS)
    tld_errmsg = tld_strerror((Tld_rc)rc);
#endif
  if(rc == TLD_INVALID)
    infof(data, "WARNING: %s; pos %u = `%c'/0x%02X\n",
          tld_errmsg, err_pos, uc_name[err_pos],
          uc_name[err_pos] & 255);
  else if(rc != TLD_SUCCESS)
    infof(data, "WARNING: TLD check for %s failed; %s\n",
          uc_name, tld_errmsg);
#endif /* CURL_DISABLE_VERBOSE_STRINGS */
  if(uc_name)
     idn_free(uc_name);
  if(rc != TLD_SUCCESS)
    return FALSE;

  return TRUE;
}