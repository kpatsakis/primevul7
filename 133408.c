void imap_mboxcache_free (IMAP_DATA* idata)
{
  LIST* cur;
  IMAP_STATUS* status;

  for (cur = idata->mboxcache; cur; cur = cur->next)
  {
    status = (IMAP_STATUS*)cur->data;

    FREE (&status->name);
  }

  mutt_free_list (&idata->mboxcache);
}