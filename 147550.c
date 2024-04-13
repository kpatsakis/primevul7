nbd_unlocked_aio_opt_go (struct nbd_handle *h,
                         nbd_completion_callback *complete)
{
  h->opt_current = NBD_OPT_GO;
  h->opt_cb.completion = *complete;
  SET_CALLBACK_TO_NULL (*complete);

  if (nbd_internal_run (h, cmd_issue) == -1)
    debug (h, "option queued, ignoring state machine failure");
  return 0;
}