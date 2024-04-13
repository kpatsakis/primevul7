int imap_status (const char* path, int queue)
{
  static int queued = 0;

  IMAP_DATA *idata;
  char buf[LONG_STRING*2];
  char mbox[LONG_STRING];
  IMAP_STATUS* status;

  if (imap_get_mailbox (path, &idata, buf, sizeof (buf)) < 0)
    return -1;

  /* We are in the folder we're polling - just return the mailbox count.
   *
   * Note that imap_mxcmp() converts NULL to "INBOX", so we need to
   * make sure the idata really is open to a folder. */
  if (idata->ctx && !imap_mxcmp (buf, idata->mailbox))
    return idata->ctx->msgcount;
  else if (mutt_bit_isset(idata->capabilities,IMAP4REV1) ||
	   mutt_bit_isset(idata->capabilities,STATUS))
  {
    imap_munge_mbox_name (idata, mbox, sizeof(mbox), buf);
    snprintf (buf, sizeof (buf), "STATUS %s (%s)", mbox, "MESSAGES");
    imap_unmunge_mbox_name (idata, mbox);
  }
  else
    /* Server does not support STATUS, and this is not the current mailbox.
     * There is no lightweight way to check recent arrivals */
    return -1;

  if (queue)
  {
    imap_exec (idata, buf, IMAP_CMD_QUEUE);
    queued = 1;
    return 0;
  }
  else if (!queued)
    imap_exec (idata, buf, 0);

  queued = 0;
  if ((status = imap_mboxcache_get (idata, mbox, 0)))
    return status->messages;

  return 0;
}