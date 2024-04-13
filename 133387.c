int imap_access (const char* path)
{
  IMAP_DATA* idata;
  IMAP_MBOX mx;
  char buf[LONG_STRING*2];
  char mailbox[LONG_STRING];
  char mbox[LONG_STRING];
  int rc;

  if (imap_parse_path (path, &mx))
    return -1;

  if (!(idata = imap_conn_find (&mx.account,
                                option (OPTIMAPPASSIVE) ? MUTT_IMAP_CONN_NONEW : 0)))
  {
    FREE (&mx.mbox);
    return -1;
  }

  imap_fix_path (idata, mx.mbox, mailbox, sizeof (mailbox));
  if (!*mailbox)
    strfcpy (mailbox, "INBOX", sizeof (mailbox));

  /* we may already be in the folder we're checking */
  if (!ascii_strcmp(idata->mailbox, mx.mbox))
  {
    FREE (&mx.mbox);
    return 0;
  }
  FREE (&mx.mbox);

  if (imap_mboxcache_get (idata, mailbox, 0))
  {
    dprint (3, (debugfile, "imap_access: found %s in cache\n", mailbox));
    return 0;
  }

  imap_munge_mbox_name (idata, mbox, sizeof (mbox), mailbox);

  if (mutt_bit_isset (idata->capabilities, IMAP4REV1))
    snprintf (buf, sizeof (buf), "STATUS %s (UIDVALIDITY)", mbox);
  else if (mutt_bit_isset (idata->capabilities, STATUS))
    snprintf (buf, sizeof (buf), "STATUS %s (UID-VALIDITY)", mbox);
  else
  {
    dprint (2, (debugfile, "imap_access: STATUS not supported?\n"));
    return -1;
  }

  if ((rc = imap_exec (idata, buf, IMAP_CMD_FAIL_OK)) < 0)
  {
    dprint (1, (debugfile, "imap_access: Can't check STATUS of %s\n", mbox));
    return rc;
  }

  return 0;
}