static int imap_check_mailbox_reopen (CONTEXT *ctx, int *index_hint)
{
  int rc;

  imap_allow_reopen (ctx);
  rc = imap_check_mailbox (ctx, index_hint, 0);
  imap_disallow_reopen (ctx);

  return rc;
}