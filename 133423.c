int imap_sync_message_for_copy (IMAP_DATA *idata, HEADER *hdr, BUFFER *cmd,
                                int *err_continue)
{
  char flags[LONG_STRING];
  char uid[11];

  if (!compare_flags_for_copy (hdr))
  {
    if (hdr->deleted == HEADER_DATA(hdr)->deleted)
      hdr->changed = 0;
    return 0;
  }

  snprintf (uid, sizeof (uid), "%u", HEADER_DATA(hdr)->uid);
  mutt_buffer_clear (cmd);
  mutt_buffer_addstr (cmd, "UID STORE ");
  mutt_buffer_addstr (cmd, uid);

  flags[0] = '\0';

  imap_set_flag (idata, MUTT_ACL_SEEN, hdr->read, "\\Seen ",
		 flags, sizeof (flags));
  imap_set_flag (idata, MUTT_ACL_WRITE, hdr->old,
                 "Old ", flags, sizeof (flags));
  imap_set_flag (idata, MUTT_ACL_WRITE, hdr->flagged,
		 "\\Flagged ", flags, sizeof (flags));
  imap_set_flag (idata, MUTT_ACL_WRITE, hdr->replied,
		 "\\Answered ", flags, sizeof (flags));
  imap_set_flag (idata, MUTT_ACL_DELETE, HEADER_DATA(hdr)->deleted,
                 "\\Deleted ", flags, sizeof (flags));

  /* now make sure we don't lose custom tags */
  if (mutt_bit_isset (idata->ctx->rights, MUTT_ACL_WRITE))
    imap_add_keywords (flags, hdr, idata->flags, sizeof (flags));

  mutt_remove_trailing_ws (flags);

  /* UW-IMAP is OK with null flags, Cyrus isn't. The only solution is to
   * explicitly revoke all system flags (if we have permission) */
  if (!*flags)
  {
    imap_set_flag (idata, MUTT_ACL_SEEN, 1, "\\Seen ", flags, sizeof (flags));
    imap_set_flag (idata, MUTT_ACL_WRITE, 1, "Old ", flags, sizeof (flags));
    imap_set_flag (idata, MUTT_ACL_WRITE, 1, "\\Flagged ", flags, sizeof (flags));
    imap_set_flag (idata, MUTT_ACL_WRITE, 1, "\\Answered ", flags, sizeof (flags));
    imap_set_flag (idata, MUTT_ACL_DELETE, !HEADER_DATA(hdr)->deleted,
                   "\\Deleted ", flags, sizeof (flags));

    mutt_remove_trailing_ws (flags);

    mutt_buffer_addstr (cmd, " -FLAGS.SILENT (");
  }
  else
    mutt_buffer_addstr (cmd, " FLAGS.SILENT (");

  mutt_buffer_addstr (cmd, flags);
  mutt_buffer_addstr (cmd, ")");

  /* after all this it's still possible to have no flags, if you
   * have no ACL rights */
  if (*flags && (imap_exec (idata, cmd->data, 0) != 0) &&
      err_continue && (*err_continue != MUTT_YES))
  {
    *err_continue = imap_continue ("imap_sync_message: STORE failed",
				   idata->buf);
    if (*err_continue != MUTT_YES)
      return -1;
  }

  if (hdr->deleted == HEADER_DATA(hdr)->deleted)
    hdr->changed = 0;

  return 0;
}