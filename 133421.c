static int imap_open_new_message (MESSAGE *msg, CONTEXT *dest, HEADER *hdr)
{
  BUFFER *tmp = NULL;
  int rc = -1;

  tmp = mutt_buffer_pool_get ();
  mutt_buffer_mktemp (tmp);
  if ((msg->fp = safe_fopen (mutt_b2s (tmp), "w")) == NULL)
  {
    mutt_perror (mutt_b2s (tmp));
    goto cleanup;
  }

  msg->path = safe_strdup (mutt_b2s (tmp));
  rc = 0;

cleanup:
  mutt_buffer_pool_release (&tmp);
  return rc;
}