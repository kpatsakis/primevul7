static int sync_helper (IMAP_DATA* idata, int right, int flag, const char* name)
{
  int count = 0;
  int rc;
  char buf[LONG_STRING];

  if (!idata->ctx)
    return -1;

  if (!mutt_bit_isset (idata->ctx->rights, right))
    return 0;

  if (right == MUTT_ACL_WRITE && !imap_has_flag (idata->flags, name))
    return 0;

  snprintf (buf, sizeof(buf), "+FLAGS.SILENT (%s)", name);
  if ((rc = imap_exec_msgset (idata, "UID STORE", buf, flag, 1, 0)) < 0)
    return rc;
  count += rc;

  buf[0] = '-';
  if ((rc = imap_exec_msgset (idata, "UID STORE", buf, flag, 1, 1)) < 0)
    return rc;
  count += rc;

  return count;
}