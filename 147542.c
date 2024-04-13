nbd_unlocked_aio_opt_list_meta_context (struct nbd_handle *h,
                                        nbd_context_callback *context,
                                        nbd_completion_callback *complete)
{
  if ((h->gflags & LIBNBD_HANDSHAKE_FLAG_FIXED_NEWSTYLE) == 0) {
    set_error (ENOTSUP, "server is not using fixed newstyle protocol");
    return -1;
  }
  if (!h->structured_replies) {
    set_error (ENOTSUP, "server lacks structured replies");
    return -1;
  }

  assert (CALLBACK_IS_NULL (h->opt_cb.fn.context));
  h->opt_cb.fn.context = *context;
  SET_CALLBACK_TO_NULL (*context);
  h->opt_cb.completion = *complete;
  SET_CALLBACK_TO_NULL (*complete);
  h->opt_current = NBD_OPT_LIST_META_CONTEXT;
  if (nbd_internal_run (h, cmd_issue) == -1)
    debug (h, "option queued, ignoring state machine failure");
  return 0;
}