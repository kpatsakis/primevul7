int imap_fast_trash (CONTEXT* ctx, char* dest)
{
  IMAP_DATA* idata;
  char mbox[LONG_STRING];
  char mmbox[LONG_STRING];
  char prompt[LONG_STRING];
  int n, rc;
  IMAP_MBOX mx;
  int triedcreate = 0;
  BUFFER *sync_cmd = NULL;
  int err_continue = MUTT_NO;

  idata = (IMAP_DATA*) ctx->data;

  if (imap_parse_path (dest, &mx))
  {
    dprint (1, (debugfile, "imap_fast_trash: bad destination %s\n", dest));
    return -1;
  }

  /* check that the save-to folder is in the same account */
  if (!mutt_account_match (&(CTX_DATA->conn->account), &(mx.account)))
  {
    dprint (3, (debugfile, "imap_fast_trash: %s not same server as %s\n",
                dest, ctx->path));
    return 1;
  }

  imap_fix_path (idata, mx.mbox, mbox, sizeof (mbox));
  if (!*mbox)
    strfcpy (mbox, "INBOX", sizeof (mbox));
  imap_munge_mbox_name (idata, mmbox, sizeof (mmbox), mbox);

  sync_cmd = mutt_buffer_new ();
  for (n = 0; n < ctx->msgcount; n++)
  {
    if (ctx->hdrs[n]->active && ctx->hdrs[n]->changed &&
        ctx->hdrs[n]->deleted && !ctx->hdrs[n]->purge)
    {
      rc = imap_sync_message_for_copy (idata, ctx->hdrs[n], sync_cmd, &err_continue);
      if (rc < 0)
      {
        dprint (1, (debugfile, "imap_fast_trash: could not sync\n"));
        goto out;
      }
    }
  }

  /* loop in case of TRYCREATE */
  do
  {
    rc = imap_exec_msgset (idata, "UID COPY", mmbox, MUTT_TRASH, 0, 0);
    if (!rc)
    {
      dprint (1, (debugfile, "imap_fast_trash: No messages to trash\n"));
      rc = -1;
      goto out;
    }
    else if (rc < 0)
    {
      dprint (1, (debugfile, "could not queue copy\n"));
      goto out;
    }
    else if (!ctx->quiet)
      mutt_message (_("Copying %d messages to %s..."), rc, mbox);

    /* let's get it on */
    rc = imap_exec (idata, NULL, IMAP_CMD_FAIL_OK);
    if (rc == -2)
    {
      if (triedcreate)
      {
        dprint (1, (debugfile, "Already tried to create mailbox %s\n", mbox));
        break;
      }
      /* bail out if command failed for reasons other than nonexistent target */
      if (ascii_strncasecmp (imap_get_qualifier (idata->buf), "[TRYCREATE]", 11))
        break;
      dprint (3, (debugfile, "imap_fast_trash: server suggests TRYCREATE\n"));
      snprintf (prompt, sizeof (prompt), _("Create %s?"), mbox);
      if (option (OPTCONFIRMCREATE) && mutt_yesorno (prompt, 1) < 1)
      {
        mutt_clear_error ();
        goto out;
      }
      if (imap_create_mailbox (idata, mbox) < 0)
        break;
      triedcreate = 1;
    }
  }
  while (rc == -2);

  if (rc != 0)
  {
    imap_error ("imap_fast_trash", idata->buf);
    goto out;
  }

  rc = 0;

out:
  mutt_buffer_free (&sync_cmd);
  FREE (&mx.mbox);

  return rc < 0 ? -1 : rc;
}