nbd_unlocked_aio_opt_abort (struct nbd_handle *h)
{
  h->opt_current = NBD_OPT_ABORT;

  if (nbd_internal_run (h, cmd_issue) == -1)
    debug (h, "option queued, ignoring state machine failure");
  return 0;
}