int imap_create_mailbox (IMAP_DATA* idata, char* mailbox)
{
  char buf[LONG_STRING*2], mbox[LONG_STRING];

  imap_munge_mbox_name (idata, mbox, sizeof (mbox), mailbox);
  snprintf (buf, sizeof (buf), "CREATE %s", mbox);

  if (imap_exec (idata, buf, 0) != 0)
  {
    mutt_error (_("CREATE failed: %s"), imap_cmd_trailer (idata));
    return -1;
  }

  return 0;
}