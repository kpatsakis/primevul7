void imap_close_connection(IMAP_DATA* idata)
{
  if (idata->state != IMAP_DISCONNECTED)
  {
    mutt_socket_close (idata->conn);
    idata->state = IMAP_DISCONNECTED;
  }
  idata->seqno = idata->nextcmd = idata->lastcmd = idata->status = 0;
  memset (idata->cmds, 0, sizeof (IMAP_COMMAND) * idata->cmdslots);
}