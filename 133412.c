static int imap_open_mailbox_append (CONTEXT *ctx, int flags)
{
  IMAP_DATA *idata;
  char buf[LONG_STRING];
  char mailbox[LONG_STRING];
  IMAP_MBOX mx;
  int rc;

  if (imap_parse_path (ctx->path, &mx))
    return -1;

  /* in APPEND mode, we appear to hijack an existing IMAP connection -
   * ctx is brand new and mostly empty */

  if (!(idata = imap_conn_find (&(mx.account), 0)))
  {
    FREE (&mx.mbox);
    return -1;
  }

  ctx->data = idata;

  imap_fix_path (idata, mx.mbox, mailbox, sizeof (mailbox));
  if (!*mailbox)
    strfcpy (mailbox, "INBOX", sizeof (mailbox));
  FREE (&mx.mbox);

  if ((rc = imap_access (ctx->path)) == 0)
    return 0;

  if (rc == -1)
    return -1;

  snprintf (buf, sizeof (buf), _("Create %s?"), mailbox);
  if (option (OPTCONFIRMCREATE) && mutt_yesorno (buf, 1) < 1)
    return -1;

  if (imap_create_mailbox (idata, mailbox) < 0)
    return -1;

  return 0;
}