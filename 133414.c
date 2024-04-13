int imap_search (CONTEXT* ctx, const pattern_t* pat)
{
  BUFFER buf;
  IMAP_DATA* idata = (IMAP_DATA*)ctx->data;
  int i;

  for (i = 0; i < ctx->msgcount; i++)
    ctx->hdrs[i]->matched = 0;

  if (!do_search (pat, 1))
    return 0;

  mutt_buffer_init (&buf);
  mutt_buffer_addstr (&buf, "UID SEARCH ");
  if (imap_compile_search (pat, &buf) < 0)
  {
    FREE (&buf.data);
    return -1;
  }
  if (imap_exec (idata, buf.data, 0) < 0)
  {
    FREE (&buf.data);
    return -1;
  }

  FREE (&buf.data);
  return 0;
}