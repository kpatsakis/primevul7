int imap_delete_mailbox (CONTEXT* ctx, IMAP_MBOX mx)
{
  char buf[LONG_STRING*2], mbox[LONG_STRING];
  IMAP_DATA *idata;

  if (!ctx || !ctx->data)
  {
    if (!(idata = imap_conn_find (&mx.account,
                                  option (OPTIMAPPASSIVE) ? MUTT_IMAP_CONN_NONEW : 0)))
    {
      FREE (&mx.mbox);
      return -1;
    }
  }
  else
  {
    idata = ctx->data;
  }

  imap_munge_mbox_name (idata, mbox, sizeof (mbox), mx.mbox);
  snprintf (buf, sizeof (buf), "DELETE %s", mbox);

  if (imap_exec ((IMAP_DATA*) idata, buf, 0) != 0)
    return -1;

  return 0;
}