int imap_subscribe (char *path, int subscribe)
{
  IMAP_DATA *idata;
  char buf[LONG_STRING*2];
  char mbox[LONG_STRING];
  int mblen;
  BUFFER err;
  IMAP_MBOX mx;

  if (!mx_is_imap (path) || imap_parse_path (path, &mx) || !mx.mbox)
  {
    mutt_error (_("Bad mailbox name"));
    return -1;
  }
  if (!(idata = imap_conn_find (&(mx.account), 0)))
    goto fail;

  imap_fix_path (idata, mx.mbox, buf, sizeof (buf));
  if (!*buf)
    strfcpy (buf, "INBOX", sizeof (buf));

  if (option (OPTIMAPCHECKSUBSCRIBED))
  {
    mutt_buffer_init (&err);
    err.dsize = STRING;
    err.data = safe_malloc (err.dsize);
    mblen = snprintf (mbox, sizeof (mbox), "%smailboxes ",
                      subscribe ? "" : "un");
    imap_quote_string_and_backquotes (mbox + mblen, sizeof(mbox) - mblen,
                                      path);
    if (mutt_parse_rc_line (mbox, &err))
      dprint (1, (debugfile, "Error adding subscribed mailbox: %s\n", err.data));
    FREE (&err.data);
  }

  if (subscribe)
    mutt_message (_("Subscribing to %s..."), buf);
  else
    mutt_message (_("Unsubscribing from %s..."), buf);
  imap_munge_mbox_name (idata, mbox, sizeof(mbox), buf);

  snprintf (buf, sizeof (buf), "%sSUBSCRIBE %s", subscribe ? "" : "UN", mbox);

  if (imap_exec (idata, buf, 0) < 0)
    goto fail;

  imap_unmunge_mbox_name(idata, mx.mbox);
  if (subscribe)
    mutt_message (_("Subscribed to %s"), mx.mbox);
  else
    mutt_message (_("Unsubscribed from %s"), mx.mbox);
  FREE (&mx.mbox);
  return 0;

fail:
  FREE (&mx.mbox);
  return -1;
}