nbd_unlocked_opt_list_meta_context (struct nbd_handle *h,
                                    nbd_context_callback *context)
{
  struct context_helper s = { .context = *context };
  nbd_context_callback l = { .callback = context_visitor, .user_data = &s };
  nbd_completion_callback c = { .callback = context_complete, .user_data = &s };

  if (nbd_unlocked_aio_opt_list_meta_context (h, &l, &c) == -1)
    return -1;

  SET_CALLBACK_TO_NULL (*context);
  if (wait_for_option (h) == -1)
    return -1;
  if (s.err) {
    set_error (s.err, "server replied with error to list meta context request");
    return -1;
  }
  return s.count;
}