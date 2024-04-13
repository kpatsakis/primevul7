IMAP_STATUS* imap_mboxcache_get (IMAP_DATA* idata, const char* mbox, int create)
{
  LIST* cur;
  IMAP_STATUS* status;
  IMAP_STATUS scache;
#ifdef USE_HCACHE
  header_cache_t *hc = NULL;
  void *puidvalidity = NULL;
  void *puidnext = NULL;
  void *pmodseq = NULL;
#endif

  for (cur = idata->mboxcache; cur; cur = cur->next)
  {
    status = (IMAP_STATUS*)cur->data;

    if (!imap_mxcmp (mbox, status->name))
      return status;
  }
  status = NULL;

  /* lame */
  if (create)
  {
    memset (&scache, 0, sizeof (scache));
    scache.name = (char*)mbox;
    idata->mboxcache = mutt_add_list_n (idata->mboxcache, &scache,
                                        sizeof (scache));
    status = imap_mboxcache_get (idata, mbox, 0);
    status->name = safe_strdup (mbox);
  }

#ifdef USE_HCACHE
  hc = imap_hcache_open (idata, mbox);
  if (hc)
  {
    puidvalidity = mutt_hcache_fetch_raw (hc, "/UIDVALIDITY", imap_hcache_keylen);
    puidnext = mutt_hcache_fetch_raw (hc, "/UIDNEXT", imap_hcache_keylen);
    pmodseq = mutt_hcache_fetch_raw (hc, "/MODSEQ", imap_hcache_keylen);
    if (puidvalidity)
    {
      if (!status)
      {
        mutt_hcache_free ((void **)&puidvalidity);
        mutt_hcache_free ((void **)&puidnext);
        mutt_hcache_free ((void **)&pmodseq);
        mutt_hcache_close (hc);
        return imap_mboxcache_get (idata, mbox, 1);
      }
      memcpy (&status->uidvalidity, puidvalidity, sizeof(unsigned int));

      if (puidnext)
        memcpy (&status->uidnext, puidnext, sizeof(unsigned int));
      else
        status->uidnext = 0;

      if (pmodseq)
        memcpy (&status->modseq, pmodseq, sizeof(unsigned long long));
      else
        status->modseq = 0;
      dprint (3, (debugfile, "mboxcache: hcache uidvalidity %u, uidnext %u, modseq %llu\n",
                  status->uidvalidity, status->uidnext, status->modseq));
    }
    mutt_hcache_free ((void **)&puidvalidity);
    mutt_hcache_free ((void **)&puidnext);
    mutt_hcache_free ((void **)&pmodseq);
    mutt_hcache_close (hc);
  }
#endif

  return status;
}