void imap_expunge_mailbox (IMAP_DATA* idata)
{
  HEADER* h;
  int i, cacheno;
  short old_sort;

#ifdef USE_HCACHE
  idata->hcache = imap_hcache_open (idata, NULL);
#endif

  old_sort = Sort;
  Sort = SORT_ORDER;
  mutt_sort_headers (idata->ctx, 0);

  for (i = 0; i < idata->ctx->msgcount; i++)
  {
    h = idata->ctx->hdrs[i];

    if (h->index == INT_MAX)
    {
      dprint (2, (debugfile, "Expunging message UID %u.\n", HEADER_DATA (h)->uid));

      h->active = 0;
      idata->ctx->size -= h->content->length;

      imap_cache_del (idata, h);
#if USE_HCACHE
      imap_hcache_del (idata, HEADER_DATA(h)->uid);
#endif

      /* free cached body from disk, if necessary */
      cacheno = HEADER_DATA(h)->uid % IMAP_CACHE_LEN;
      if (idata->cache[cacheno].uid == HEADER_DATA(h)->uid &&
	  idata->cache[cacheno].path)
      {
	unlink (idata->cache[cacheno].path);
	FREE (&idata->cache[cacheno].path);
      }

      int_hash_delete (idata->uid_hash, HEADER_DATA(h)->uid, h, NULL);

      imap_free_header_data ((IMAP_HEADER_DATA**)&h->data);
    }
    else
    {
      h->index = i;
      /* Mutt has several places where it turns off h->active as a
       * hack.  For example to avoid FLAG updates, or to exclude from
       * imap_exec_msgset.
       *
       * Unfortunately, when a reopen is allowed and the IMAP_EXPUNGE_PENDING
       * flag becomes set (e.g. a flag update to a modified header),
       * this function will be called by imap_cmd_finish().
       *
       * The mx_update_tables() will free and remove these "inactive" headers,
       * despite that an EXPUNGE was not received for them.
       * This would result in memory leaks and segfaults due to dangling
       * pointers in the msn_index and uid_hash.
       *
       * So this is another hack to work around the hacks.  We don't want to
       * remove the messages, so make sure active is on.
       */
      h->active = 1;
    }
  }

#if USE_HCACHE
  imap_hcache_close (idata);
#endif

  /* We may be called on to expunge at any time. We can't rely on the caller
   * to always know to rethread */
  mx_update_tables (idata->ctx, 0);
  Sort = old_sort;
  mutt_sort_headers (idata->ctx, 1);
}