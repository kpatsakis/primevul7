static int imap_save_to_header_cache (CONTEXT *ctx, HEADER *h)
{
  int rc = 0;
#ifdef USE_HCACHE
  int close_hc = 1;
  IMAP_DATA* idata;

  idata = (IMAP_DATA *)ctx->data;
  if (idata->hcache)
    close_hc = 0;
  else
    idata->hcache = imap_hcache_open (idata, NULL);
  rc = imap_hcache_put (idata, h);
  if (close_hc)
    imap_hcache_close (idata);
#endif
  return rc;
}