int imap_rename_mailbox (IMAP_DATA* idata, IMAP_MBOX* mx, const char* newname)
{
  char oldmbox[LONG_STRING];
  char newmbox[LONG_STRING];
  BUFFER *b;
  int rc = 0;

  imap_munge_mbox_name (idata, oldmbox, sizeof (oldmbox), mx->mbox);
  imap_munge_mbox_name (idata, newmbox, sizeof (newmbox), newname);

  b = mutt_buffer_pool_get ();
  mutt_buffer_printf (b, "RENAME %s %s", oldmbox, newmbox);

  if (imap_exec (idata, mutt_b2s (b), 0) != 0)
    rc = -1;

  mutt_buffer_pool_release (&b);

  return rc;
}