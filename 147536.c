nbd_unlocked_aio_opt_info (struct nbd_handle *h,
                           nbd_completion_callback *complete)
{
  if ((h->gflags & LIBNBD_HANDSHAKE_FLAG_FIXED_NEWSTYLE) == 0) {
    set_error (ENOTSUP, "server is not using fixed newstyle protocol");
    return -1;
  }

  h->opt_current = NBD_OPT_INFO;
  h->opt_cb.completion = *complete;
  SET_CALLBACK_TO_NULL (*complete);

  if (nbd_internal_run (h, cmd_issue) == -1)
    debug (h, "option queued, ignoring state machine failure");
  return 0;
}