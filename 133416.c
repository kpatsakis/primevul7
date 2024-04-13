static int imap_get_mailbox (const char* path, IMAP_DATA** hidata, char* buf, size_t blen)
{
  IMAP_MBOX mx;

  if (imap_parse_path (path, &mx))
  {
    dprint (1, (debugfile, "imap_get_mailbox: Error parsing %s\n", path));
    return -1;
  }
  if (!(*hidata = imap_conn_find (&(mx.account), option (OPTIMAPPASSIVE) ? MUTT_IMAP_CONN_NONEW : 0)))
  {
    FREE (&mx.mbox);
    return -1;
  }

  imap_fix_path (*hidata, mx.mbox, buf, blen);
  if (!*buf)
    strfcpy (buf, "INBOX", blen);
  FREE (&mx.mbox);

  return 0;
}