static int imap_open_mailbox (CONTEXT* ctx)
{
  IMAP_DATA *idata;
  IMAP_STATUS* status;
  char buf[LONG_STRING];
  char bufout[LONG_STRING*2];
  int count = 0;
  IMAP_MBOX mx, pmx;
  int rc;
  const char *condstore;

  if (imap_parse_path (ctx->path, &mx))
  {
    mutt_error (_("%s is an invalid IMAP path"), ctx->path);
    return -1;
  }

  /* we require a connection which isn't currently in IMAP_SELECTED state */
  if (!(idata = imap_conn_find (&(mx.account), MUTT_IMAP_CONN_NOSELECT)))
    goto fail_noidata;

  /* once again the context is new */
  ctx->data = idata;

  /* Clean up path and replace the one in the ctx */
  imap_fix_path (idata, mx.mbox, buf, sizeof (buf));
  if (!*buf)
    strfcpy (buf, "INBOX", sizeof (buf));
  FREE(&(idata->mailbox));
  idata->mailbox = safe_strdup (buf);
  imap_qualify_path (buf, sizeof (buf), &mx, idata->mailbox);

  FREE (&(ctx->path));
  FREE (&(ctx->realpath));
  ctx->path = safe_strdup (buf);
  ctx->realpath = safe_strdup (ctx->path);

  idata->ctx = ctx;

  /* clear mailbox status */
  idata->status = 0;
  memset (idata->ctx->rights, 0, sizeof (idata->ctx->rights));
  idata->newMailCount = 0;
  idata->max_msn = 0;

  if (!ctx->quiet)
    mutt_message (_("Selecting %s..."), idata->mailbox);
  imap_munge_mbox_name (idata, buf, sizeof(buf), idata->mailbox);

  /* pipeline ACL test */
  if (mutt_bit_isset (idata->capabilities, ACL))
  {
    snprintf (bufout, sizeof (bufout), "MYRIGHTS %s", buf);
    imap_exec (idata, bufout, IMAP_CMD_QUEUE);
  }
  /* assume we have all rights if ACL is unavailable */
  else
  {
    mutt_bit_set (idata->ctx->rights, MUTT_ACL_LOOKUP);
    mutt_bit_set (idata->ctx->rights, MUTT_ACL_READ);
    mutt_bit_set (idata->ctx->rights, MUTT_ACL_SEEN);
    mutt_bit_set (idata->ctx->rights, MUTT_ACL_WRITE);
    mutt_bit_set (idata->ctx->rights, MUTT_ACL_INSERT);
    mutt_bit_set (idata->ctx->rights, MUTT_ACL_POST);
    mutt_bit_set (idata->ctx->rights, MUTT_ACL_CREATE);
    mutt_bit_set (idata->ctx->rights, MUTT_ACL_DELETE);
  }
  /* pipeline the postponed count if possible */
  pmx.mbox = NULL;
  if (mx_is_imap (Postponed) && !imap_parse_path (Postponed, &pmx)
      && mutt_account_match (&pmx.account, &mx.account))
    imap_status (Postponed, 1);
  FREE (&pmx.mbox);

#if USE_HCACHE
  if (mutt_bit_isset (idata->capabilities, CONDSTORE) &&
      option (OPTIMAPCONDSTORE))
    condstore = " (CONDSTORE)";
  else
#endif
    condstore = "";

  snprintf (bufout, sizeof (bufout), "%s %s%s",
            ctx->readonly ? "EXAMINE" : "SELECT",
            buf, condstore);

  idata->state = IMAP_SELECTED;

  imap_cmd_start (idata, bufout);

  status = imap_mboxcache_get (idata, idata->mailbox, 1);

  do
  {
    char *pc;

    if ((rc = imap_cmd_step (idata)) != IMAP_CMD_CONTINUE)
      break;

    pc = idata->buf + 2;

    /* Obtain list of available flags here, may be overridden by a
     * PERMANENTFLAGS tag in the OK response */
    if (ascii_strncasecmp ("FLAGS", pc, 5) == 0)
    {
      /* don't override PERMANENTFLAGS */
      if (!idata->flags)
      {
	dprint (3, (debugfile, "Getting mailbox FLAGS\n"));
	if ((pc = imap_get_flags (&(idata->flags), pc)) == NULL)
	  goto fail;
      }
    }
    /* PERMANENTFLAGS are massaged to look like FLAGS, then override FLAGS */
    else if (ascii_strncasecmp ("OK [PERMANENTFLAGS", pc, 18) == 0)
    {
      dprint (3, (debugfile, "Getting mailbox PERMANENTFLAGS\n"));
      /* safe to call on NULL */
      mutt_free_list (&(idata->flags));
      /* skip "OK [PERMANENT" so syntax is the same as FLAGS */
      pc += 13;
      if ((pc = imap_get_flags (&(idata->flags), pc)) == NULL)
	goto fail;
    }
    /* save UIDVALIDITY for the header cache */
    else if (ascii_strncasecmp ("OK [UIDVALIDITY", pc, 14) == 0)
    {
      dprint (3, (debugfile, "Getting mailbox UIDVALIDITY\n"));
      pc += 3;
      pc = imap_next_word (pc);
      if (mutt_atoui (pc, &idata->uid_validity) < 0)
        goto fail;
      status->uidvalidity = idata->uid_validity;
    }
    else if (ascii_strncasecmp ("OK [UIDNEXT", pc, 11) == 0)
    {
      dprint (3, (debugfile, "Getting mailbox UIDNEXT\n"));
      pc += 3;
      pc = imap_next_word (pc);
      if (mutt_atoui (pc, &idata->uidnext) < 0)
        goto fail;
      status->uidnext = idata->uidnext;
    }
    else if (ascii_strncasecmp ("OK [HIGHESTMODSEQ", pc, 17) == 0)
    {
      dprint (3, (debugfile, "Getting mailbox HIGHESTMODSEQ\n"));
      pc += 3;
      pc = imap_next_word (pc);
      if (mutt_atoull (pc, &idata->modseq) < 0)
        goto fail;
      status->modseq = idata->modseq;
    }
    else if (ascii_strncasecmp ("OK [NOMODSEQ", pc, 12) == 0)
    {
      dprint (3, (debugfile, "Mailbox has NOMODSEQ set\n"));
      status->modseq = idata->modseq = 0;
    }
    else
    {
      pc = imap_next_word (pc);
      if (!ascii_strncasecmp ("EXISTS", pc, 6))
      {
	count = idata->newMailCount;
	idata->newMailCount = 0;
      }
    }
  }
  while (rc == IMAP_CMD_CONTINUE);

  if (rc == IMAP_CMD_NO)
  {
    char *s;
    s = imap_next_word (idata->buf); /* skip seq */
    s = imap_next_word (s); /* Skip response */
    mutt_error ("%s", s);
    mutt_sleep (2);
    goto fail;
  }

  if (rc != IMAP_CMD_OK)
    goto fail;

  /* check for READ-ONLY notification */
  if (!ascii_strncasecmp (imap_get_qualifier (idata->buf), "[READ-ONLY]", 11) &&
      !mutt_bit_isset (idata->capabilities, ACL))
  {
    dprint (2, (debugfile, "Mailbox is read-only.\n"));
    ctx->readonly = 1;
  }

#ifdef DEBUG
  /* dump the mailbox flags we've found */
  if (debuglevel > 2)
  {
    if (!idata->flags)
      dprint (3, (debugfile, "No folder flags found\n"));
    else
    {
      LIST* t = idata->flags;

      dprint (3, (debugfile, "Mailbox flags: "));

      t = t->next;
      while (t)
      {
        dprint (3, (debugfile, "[%s] ", t->data));
        t = t->next;
      }
      dprint (3, (debugfile, "\n"));
    }
  }
#endif

  if (!(mutt_bit_isset(idata->ctx->rights, MUTT_ACL_DELETE) ||
        mutt_bit_isset(idata->ctx->rights, MUTT_ACL_SEEN) ||
        mutt_bit_isset(idata->ctx->rights, MUTT_ACL_WRITE) ||
        mutt_bit_isset(idata->ctx->rights, MUTT_ACL_INSERT)))
    ctx->readonly = 1;

  ctx->hdrmax = count;
  ctx->hdrs = safe_calloc (count, sizeof (HEADER *));
  ctx->v2r = safe_calloc (count, sizeof (int));
  ctx->msgcount = 0;

  if (count && (imap_read_headers (idata, 1, count, 1) < 0))
  {
    mutt_error _("Error opening mailbox");
    mutt_sleep (1);
    goto fail;
  }

  imap_disallow_reopen (ctx);

  dprint (2, (debugfile, "imap_open_mailbox: msgcount is %d\n", ctx->msgcount));
  FREE (&mx.mbox);
  return 0;

fail:
  if (idata->state == IMAP_SELECTED)
    idata->state = IMAP_AUTHENTICATED;
fail_noidata:
  FREE (&mx.mbox);
  return -1;
}