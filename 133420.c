int imap_complete(char* dest, size_t dlen, const char* path)
{
  IMAP_DATA* idata;
  char list[LONG_STRING];
  char buf[LONG_STRING*2];
  IMAP_LIST listresp;
  char completion[LONG_STRING];
  int clen;
  size_t matchlen = 0;
  int completions = 0;
  IMAP_MBOX mx;
  int rc;

  if (imap_parse_path (path, &mx))
  {
    strfcpy (dest, path, dlen);
    return imap_complete_hosts (dest, dlen);
  }

  /* don't open a new socket just for completion. Instead complete over
   * known mailboxes/hooks/etc */
  if (!(idata = imap_conn_find (&(mx.account), MUTT_IMAP_CONN_NONEW)))
  {
    FREE (&mx.mbox);
    strfcpy (dest, path, dlen);
    return imap_complete_hosts (dest, dlen);
  }

  /* reformat path for IMAP list, and append wildcard */
  /* don't use INBOX in place of "" */
  if (mx.mbox && mx.mbox[0])
    imap_fix_path (idata, mx.mbox, list, sizeof(list));
  else
    list[0] = '\0';

  /* fire off command */
  snprintf (buf, sizeof(buf), "%s \"\" \"%s%%\"",
            option (OPTIMAPLSUB) ? "LSUB" : "LIST", list);

  imap_cmd_start (idata, buf);

  /* and see what the results are */
  strfcpy (completion, NONULL(mx.mbox), sizeof(completion));
  idata->cmdtype = IMAP_CT_LIST;
  idata->cmddata = &listresp;
  do
  {
    listresp.name = NULL;
    rc = imap_cmd_step (idata);

    if (rc == IMAP_CMD_CONTINUE && listresp.name)
    {
      /* if the folder isn't selectable, append delimiter to force browse
       * to enter it on second tab. */
      if (listresp.noselect)
      {
        clen = strlen(listresp.name);
        listresp.name[clen++] = listresp.delim;
        listresp.name[clen] = '\0';
      }
      /* copy in first word */
      if (!completions)
      {
        strfcpy (completion, listresp.name, sizeof(completion));
        matchlen = strlen (completion);
        completions++;
        continue;
      }

      matchlen = longest_common_prefix (completion, listresp.name, 0, matchlen);
      completions++;
    }
  }
  while (rc == IMAP_CMD_CONTINUE);
  idata->cmddata = NULL;

  if (completions)
  {
    /* reformat output */
    imap_qualify_path (dest, dlen, &mx, completion);
    mutt_pretty_mailbox (dest, dlen);

    FREE (&mx.mbox);
    return 0;
  }

  return -1;
}