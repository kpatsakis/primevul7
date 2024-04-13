static int imap_make_msg_set (IMAP_DATA* idata, BUFFER* buf, int flag,
                              int changed, int invert, int* pos)
{
  HEADER** hdrs = idata->ctx->hdrs;
  int count = 0;	/* number of messages in message set */
  int match = 0;	/* whether current message matches flag condition */
  unsigned int setstart = 0;	/* start of current message range */
  int n;
  int started = 0;

  hdrs = idata->ctx->hdrs;

  for (n = *pos;
       (n < idata->ctx->msgcount) && (mutt_buffer_len (buf) < IMAP_MAX_CMDLEN);
       n++)
  {
    match = 0;
    /* don't include pending expunged messages.
     *
     * TODO: can we unset active in cmd_parse_expunge() and
     * cmd_parse_vanished() instead of checking for index != INT_MAX. */
    if (hdrs[n]->active && (hdrs[n]->index != INT_MAX))
      switch (flag)
      {
        case MUTT_DELETED:
          if (hdrs[n]->deleted != HEADER_DATA(hdrs[n])->deleted)
            match = invert ^ hdrs[n]->deleted;
	  break;
        case MUTT_FLAG:
          if (hdrs[n]->flagged != HEADER_DATA(hdrs[n])->flagged)
            match = invert ^ hdrs[n]->flagged;
	  break;
        case MUTT_OLD:
          if (hdrs[n]->old != HEADER_DATA(hdrs[n])->old)
            match = invert ^ hdrs[n]->old;
	  break;
        case MUTT_READ:
          if (hdrs[n]->read != HEADER_DATA(hdrs[n])->read)
            match = invert ^ hdrs[n]->read;
	  break;
        case MUTT_REPLIED:
          if (hdrs[n]->replied != HEADER_DATA(hdrs[n])->replied)
            match = invert ^ hdrs[n]->replied;
	  break;

        case MUTT_TAG:
	  if (hdrs[n]->tagged)
	    match = 1;
	  break;
        case MUTT_TRASH:
          if (hdrs[n]->deleted && !hdrs[n]->purge)
            match = 1;
	  break;
      }

    if (match && (!changed || hdrs[n]->changed))
    {
      count++;
      if (setstart == 0)
      {
        setstart = HEADER_DATA (hdrs[n])->uid;
        if (started == 0)
	{
	  mutt_buffer_add_printf (buf, "%u", HEADER_DATA (hdrs[n])->uid);
	  started = 1;
	}
        else
	  mutt_buffer_add_printf (buf, ",%u", HEADER_DATA (hdrs[n])->uid);
      }
      /* tie up if the last message also matches */
      else if (n == idata->ctx->msgcount-1)
	mutt_buffer_add_printf (buf, ":%u", HEADER_DATA (hdrs[n])->uid);
    }
    /* End current set if message doesn't match or we've reached the end
     * of the mailbox via inactive messages following the last match. */
    else if (setstart && (hdrs[n]->active || n == idata->ctx->msgcount-1))
    {
      if (HEADER_DATA (hdrs[n-1])->uid > setstart)
	mutt_buffer_add_printf (buf, ":%u", HEADER_DATA (hdrs[n-1])->uid);
      setstart = 0;
    }
  }

  *pos = n;

  return count;
}